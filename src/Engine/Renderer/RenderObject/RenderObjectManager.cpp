#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>


#include <Engine/RadiumEngine.hpp>
#include <Engine/Component/Component.hpp>

namespace Ra
{
    namespace Engine
    {
        RenderObjectManager::RenderObjectManager()
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
            Core::Index index = m_renderObjects.insert( newRenderObject );

            newRenderObject->idx = index;

            auto type = renderObject->getType();

            m_renderObjectByType[(int)type].insert( index );

            if (type == RenderObjectType::Fancy)
                m_fancyBVH.insertLeaf(newRenderObject);

            Engine::RadiumEngine::getInstance()->getSignalManager()->fireRenderObjectAdded(
                    ItemEntry( renderObject->getComponent()->getEntity(),
                               renderObject->getComponent(),
                               index ));
            return index;
        }

        void RenderObjectManager::removeRenderObject( const Core::Index& index )
        {
            CORE_ASSERT( exists (index), "Trying to access a render object which doesn't exist");

            // FIXME(Charly): Should we check if the render object is in the double buffer map ?
            std::shared_ptr<RenderObject> renderObject = m_renderObjects.at( index );


            Engine::RadiumEngine::getInstance()->getSignalManager()->fireRenderObjectRemoved(
                    ItemEntry( renderObject->getComponent()->getEntity(),
                               renderObject->getComponent(),
                               index ));

            // Lock after signal has been fired (as this signal can cause another RO to be deleted)
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );
            m_renderObjects.remove( index );
            auto type = renderObject->getType();
            m_renderObjectByType[(int)type].erase( index );
            renderObject.reset();
        }

        std::shared_ptr<RenderObject> RenderObjectManager::getRenderObject( const Core::Index& index )
        {
            CORE_ASSERT( exists (index), "Trying to access a render object which doesn't exist");
            return m_renderObjects.at( index );
        }

        void RenderObjectManager::getRenderObjects( std::vector<std::shared_ptr<RenderObject>>& renderObjectsOut ) const
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            // Copy each element in m_renderObjects
            for ( uint i = 0; i < m_renderObjects.size(); ++i )
            {
                renderObjectsOut.push_back( m_renderObjects.at( i ) );
            }
        }

        void RenderObjectManager::getRenderObjectsByType( const RenderData& renderData,
                                                          std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                                          const RenderObjectType& type ) const
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            /*if (type == RenderObjectType::Fancy)
            {
                Core::Matrix4 mvp(renderData.projMatrix * renderData.viewMatrix);
                m_fancyBVH.update();
                m_fancyBVH.getInFrustumSlow(objectsOut, Core::Frustum(mvp));
            }
            else*/
            {
                //// Copy each element in m_renderObjects
                for ( const auto& idx : m_renderObjectByType[(int)type] )
                {
                    objectsOut.push_back( m_renderObjects.at( idx ) );
                }
            }
        }

        void RenderObjectManager::renderObjectExpired( const Core::Index& idx )
        {
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            auto ro = m_renderObjects.at( idx );
            m_renderObjects.remove( idx );

            auto type = ro->getType();

            m_renderObjectByType[(int)type].erase( idx );

            ro->hasExpired();

            ro.reset();
        }
    }
} // namespace Ra
