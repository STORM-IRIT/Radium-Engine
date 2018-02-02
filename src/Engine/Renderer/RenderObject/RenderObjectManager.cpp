#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>


#include <Engine/RadiumEngine.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <Engine/Managers/SignalManager/SignalManager.hpp>

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
            return (index.isValid() &&  m_renderObjects.contains( index ) );
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

        uint RenderObjectManager::getRenderObjectsCount()
        {
            return m_renderObjects.size();
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

            //// Copy each element in m_renderObjects
            for ( const auto& idx : m_renderObjectByType[(int)type] )
            {
                objectsOut.push_back( m_renderObjects.at( idx ) );
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

        uint RenderObjectManager::getNumFaces() const
        {
            uint result = 0;
            for (const auto& ro : m_renderObjects)
            {
                if (ro->isVisible() && ro->getType() == Ra::Engine::RenderObjectType::Fancy )
                {
                    result += ro->getMesh()->getGeometry().m_triangles.size();
                }
            }
            return result;
        }

        uint RenderObjectManager::getNumVertices() const
        {
            uint result = 0;
            for (const auto& ro : m_renderObjects)
            {
                if (ro->isVisible() && ro->getType() == Ra::Engine::RenderObjectType::Fancy )
                {
                    result += ro->getMesh()->getGeometry().m_vertices.size();
                }
            }
            return result;
        }

        Core::Aabb RenderObjectManager::getSceneAabb() const
        {
            Core::Aabb aabb;

            auto ui = Engine::SystemEntity::uiCmp();
            bool skipUi = m_renderObjects.size() != ui->m_renderObjects.size();
            for ( auto ro: m_renderObjects)
            {
                if (ro->isVisible() && (!skipUi || ro->getComponent() != ui))
                {
                    Core::Transform t = ro->getComponent()->getEntity()->getTransform();
                    auto mesh = ro->getMesh();
                    auto pos = mesh->getGeometry().m_vertices;

                    for (auto& p : pos)
                    {
                        p = t * ro->getLocalTransform() * p;
                    }

                    const Ra::Core::Vector3 bmin = pos.getMap().rowwise().minCoeff().head<3>();
                    const Ra::Core::Vector3 bmax = pos.getMap().rowwise().maxCoeff().head<3>();

                    aabb.extend(bmin);
                    aabb.extend(bmax);
                }
            }
            return aabb;
        }
    }
} // namespace Ra
