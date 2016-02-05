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

            return idx;
        }

        void RenderObjectManager::removeRenderObject( const Core::Index& index )
        {
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
            std::shared_ptr<RenderObject> ret = nullptr;
            if ( index != Core::Index::INVALID_IDX() )
            {
                ret = m_renderObjects.at( index );
            }

            return ret;
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

        void RenderObjectManager::getRenderObjectsByTypeIfDirty( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
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


        void RenderObjectManager::getRenderObjectsByType( std::vector<std::shared_ptr<RenderObject>>& objectsOut,
                                                          const RenderObjectType& type, bool undirty ) const
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            // Copy each element in m_renderObjects
            for ( const auto& idx : m_renderObjectByType[(int)type] )
            {
                objectsOut.push_back( m_renderObjects.at( idx ) );
            }

            if ( undirty )
            {
                m_typeIsDirty[(int)type] = false;
            }
        }

        std::shared_ptr<RenderObject> RenderObjectManager::update( uint index, bool cloneMesh )
        {
            Core::Index idx( index );

            return update( idx, cloneMesh );
        }

        std::shared_ptr<RenderObject> RenderObjectManager::update( const Core::Index& index, bool cloneMesh )
        {

            CORE_ASSERT( index.isValid() ,"Invalid index" );

            // A render object should never be updated if it is already in use.
            // It might be :
            //     - update called by another component
            //     - doneUpdating not called on a previous update
            if ( m_doubleBuffer.find( index ) != m_doubleBuffer.end() )
            {
                CORE_ERROR( "The render object required for update is already "
                            "being updated by someone else. "
                            "Maybe you forgot to call RenderObjectManager::doneUpdating() ?" );
            }

            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            // Clone the render object
            std::shared_ptr<RenderObject> copy = std::shared_ptr<RenderObject> (
                                                     m_renderObjects[index]->clone( cloneMesh ) );

            // Store it in the double buffer map
            m_doubleBuffer[index] = copy;
            return copy;
        }

        void RenderObjectManager::doneUpdating( uint index )
        {
            // Take the mutex
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            m_doneUpdatingObjects.push_back( index );
        }

        void RenderObjectManager::swapBuffers()
        {
            for ( const auto& index : m_doneUpdatingObjects )
            {
                std::shared_ptr<RenderObject> oldRenderObject = m_renderObjects[index];
                std::shared_ptr<RenderObject> newRenderObject = m_doubleBuffer[index];

                // We delete the old render object. If it is still used by the renderer,
                // the pointer will still be valid for it.
                oldRenderObject.reset();

                // Buffer swapping
                m_renderObjects[index] = newRenderObject;

                // Remove the double buffer entry
                m_doubleBuffer.erase( index );
            }

            m_doneUpdatingObjects.clear();
        }

        bool RenderObjectManager::isDirty() const
        {
            std::lock_guard<std::mutex> lock( m_doubleBufferMutex );

            bool isDirty = false;
            for ( const auto& b : m_typeIsDirty )
            {
                isDirty |= b;
            }

            return isDirty;
        }
    }
} // namespace Ra
