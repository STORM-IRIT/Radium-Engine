#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <Core/CoreMacros.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra
{
    namespace Engine
    {
        RenderObjectManager::RenderObjectManager()
            : m_typeIsDirty{{ true }}
        {
        }

        RenderObjectManager::~RenderObjectManager()
        {
        }

        bool RenderObjectManager::exists( const Core::Index& index ) const
        {
            return (index != Core::Index::INVALID_IDX()) &&
                    m_renderObjects.contain( index );
        }

        Core::Index RenderObjectManager::addRenderObject( RenderObject* renderObject )
        {
            // Avoid data race in the std::maps
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            std::shared_ptr<RenderObject> newRenderObject( renderObject );
            Core::Index idx = m_renderObjects.insert( newRenderObject );

            newRenderObject->idx = idx;

            auto type = renderObject->getType();

            m_renderObjectByType[(int)type].insert( idx );
            m_typeIsDirty[(int)type] = true;

            if (type == RenderObjectType::Fancy)
                m_fancyBVH.insertLeaf(newRenderObject);

            return idx;
        }

        void RenderObjectManager::removeRenderObject( const Core::Index& index )
        {
            CORE_ASSERT( exists (index), "Trying to access a render object which doesn't exist");
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            // FIXME(Charly): Should we check if the render object is in the double buffer map ?
            std::shared_ptr<RenderObject> renderObject = m_renderObjects.at( index );
            m_renderObjects.remove( index );

            auto type = renderObject->getType();

            m_renderObjectByType[(int)type].erase( index );
            m_typeIsDirty[(int)type] = true;

            renderObject.reset();
        }

        std::shared_ptr<RenderObject> RenderObjectManager::getRenderObject( const Core::Index& index )
        {
            CORE_ASSERT( exists (index), "Trying to access a render object which doesn't exist");
            return m_renderObjects.at( index );
        }

        void RenderObjectManager::getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& renderObjectsOut, bool undirty ) const
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            // Copy each element in m_renderObjects

            for ( uint i = 0; i < m_renderObjects.size(); ++i )
            {
                renderObjectsOut.push_back( m_renderObjects.at( i ) );
            }

            if ( undirty )
            {
                for ( auto& b : m_typeIsDirty )
                {
                    b = false;
                }
            }
        }

        void RenderObjectManager::getRenderObjectsByTypeIfDirty( const RenderData& renderData,
                                                                 std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                                                 const RenderObjectType& type, bool undirty ) const
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            // Copy each element in m_renderObjects

            if ( !m_typeIsDirty[(int)type] )
            {
                return;
            }

            // Not dirty, clear the vector
            objectsOut.clear();

            for ( const auto& idx : m_renderObjectByType[(int)type] )
            {
                objectsOut.push_back( m_renderObjects.at( idx ) );
            }

            if ( undirty )
            {
                m_typeIsDirty[(int)type] = false;
            }
        }


        void RenderObjectManager::getRenderObjectsByType( const RenderData& renderData,
                                                          std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                                          const RenderObjectType& type, bool undirty ) const
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            if (type == RenderObjectType::Fancy)
            {
                Core::Matrix4 mvp(renderData.projMatrix * renderData.viewMatrix);

                //std::cout << mvp << std::endl;

                m_fancyBVH.update();
                m_fancyBVH.getInFrustumSlow(objectsOut, Core::Frustum(mvp));

                // DEBUG
                LOG(logINFO) << "Objets dans le frustum / Total : " << objectsOut.size() << "/" << m_renderObjectByType[(int)RenderObjectType::Fancy].size() ;
            }
            else
            {
                // DEBUG
                if (type == RenderObjectType::Debug)
                {
                    Core::Matrix4 mvp(renderData.projMatrix * renderData.viewMatrix);
                    m_fancyBVH.update();
                    m_fancyBVH.getNotInFrustumSlow(objectsOut, Core::Frustum(mvp));
                }

                // Copy each element in m_renderObjects
                for ( const auto& idx : m_renderObjectByType[(int)type] )
                {
                    objectsOut.push_back( m_renderObjects.at( idx ) );
                }
            }

            if ( undirty )
            {
                m_typeIsDirty[(int)type] = false;
            }
        }

        bool RenderObjectManager::isDirty() const
        {
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            bool isDirty = false;
            for ( const auto& b : m_typeIsDirty )
            {
                isDirty = (isDirty || b) ;
            }

            return isDirty;
        }

        void RenderObjectManager::renderObjectExpired( const Core::Index& idx )
        {
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            auto ro = m_renderObjects.at( idx );
            m_renderObjects.remove( idx );

            auto type = ro->getType();

            m_renderObjectByType[(int)type].erase( idx );
            m_typeIsDirty[(int)type] = true;

            ro->hasExpired();

            ro.reset();
        }
    }
} // namespace Ra
