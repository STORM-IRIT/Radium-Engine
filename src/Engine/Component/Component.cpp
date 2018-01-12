#include <Engine/Component/Component.hpp>

#include <Core/Log/Log.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

namespace Ra
{
    namespace Engine
    {
        Component::Component( const std::string& name )
                : m_name( name )
                , m_entity( nullptr )
                , m_system( nullptr )
        {
        }

        Component::~Component()
        {
            for (const auto& ro : m_renderObjects )
            {
                getRoMgr()->removeRenderObject( ro );
            }
            if (m_system)
            {
                m_system->unregisterComponent(getEntity(), this);
            }
            RadiumEngine::getInstance()->getSignalManager()->fireComponentRemoved( ItemEntry(getEntity(),this));
        }

        RenderObjectManager* Component::getRoMgr()
        {
            return RadiumEngine::getInstance()->getRenderObjectManager();
        }

        Core::Index Component::addRenderObject( RenderObject* renderObject )
        {
            m_renderObjects.push_back( getRoMgr()->addRenderObject( renderObject ) );
            return m_renderObjects.back();
        }

        void Component::removeRenderObject( Core::Index roIdx )
        {
            auto found = std::find(m_renderObjects.cbegin(), m_renderObjects.cend(),roIdx);
            CORE_WARN_IF(found == m_renderObjects.cend(), " Render object not found in component");
            if ( (found != m_renderObjects.cend()) && getRoMgr() )
            {
                getRoMgr()->removeRenderObject(*found);
                m_renderObjects.erase(found);
            }
        }

        void Component::notifyRenderObjectExpired( const Core::Index& idx )
        {
            auto found = std::find( m_renderObjects.cbegin(), m_renderObjects.cend(), idx );
            CORE_WARN_IF( found == m_renderObjects.cend(), " Render object not found in component" );
            if ( found != m_renderObjects.cend() )
            {
                m_renderObjects.erase( found );
            }
        }

        void Component::rayCastQuery(const Core::Ray& ray) const
        {
            for (const auto& idx : m_renderObjects)
            {
                const auto ro = getRoMgr()->getRenderObject(idx);
                if (ro->isVisible())
                {
                    const Ra::Core::Transform& t = ro->getLocalTransform();
                    Core::Ray transformedRay = Ra::Core::transformRay(ray, t.inverse());
                    auto result = Ra::Core::MeshUtils::castRay(ro->getMesh()->getGeometry(), transformedRay);
                    const int& tidx = result.m_hitTriangle;
                    if (tidx >= 0)
                    {

                        const Ra::Core::Vector3 pLocal = transformedRay.pointAt(result.m_t);
                        const Ra::Core::Vector3 pEntity = t * pLocal;
                        const Ra::Core::Vector3 pWorld = getEntity()->getTransform() * pEntity;

                        LOG(logINFO) << " Ray cast vs " << ro->getName();
                        LOG(logINFO) << " Hit triangle " << tidx;
                        LOG(logINFO) << " Nearest vertex " << result.m_nearestVertex;
                        LOG(logINFO) << "Hit position (RO): " << pLocal.transpose();
                        LOG(logINFO) << "Hit position (Comp): " << pEntity.transpose();
                        LOG(logINFO) << "Hit position (World): " << pWorld.transpose();
                    }
                }
            }

        }
    }
} // namespace Ra
