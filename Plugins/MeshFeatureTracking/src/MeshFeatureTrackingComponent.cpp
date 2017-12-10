#include <MeshFeatureTrackingComponent.hpp>

#include <queue>
#include <iostream>

#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

using Ra::Engine::ComponentMessenger;

namespace MeshFeatureTrackingPlugin
{
    MeshFeatureTrackingComponent::MeshFeatureTrackingComponent(const std::string& name) :
        Component(name)
    {}

    MeshFeatureTrackingComponent::~MeshFeatureTrackingComponent()
    {}

    void MeshFeatureTrackingComponent::initialize()
    {
        std::shared_ptr<Ra::Engine::Mesh> display( new Ra::Engine::Mesh("FeaturePickingManagerSphere") );
        auto sphere = Ra::Core::MeshUtils::makeParametricSphere(1.0);
        display->loadGeometry( sphere );
        std::shared_ptr<Ra::Engine::Material> material;
        material.reset( new Ra::Engine::Material("VertexPickingManageSphereMaterial") );
        material-> m_kd = Ra::Core::Color(0.f,1.f,0.f,1.f);
        m_RO = Ra::Engine::RenderObject::createRenderObject( "FeaturePickingManagerSphereRO", this,
                                                             Ra::Engine::RenderObjectType::Fancy, display,
 //                                                            Ra::Engine::ShaderConfigurationFactory::getConfiguration("BlinnPhong"),
                                                             Ra::Engine::RenderTechnique::createDefaultRenderTechnique(),
                                                             material );
        m_RO->setPickable( false );
        m_RO->setVisible( false );
        addRenderObject( m_RO );
    }

    void MeshFeatureTrackingComponent::setPosition( Ra::Core::Vector3 position )
    {
        Ra::Core::Translation aa( position );
        Ra::Core::Transform rot( aa );
        m_RO->setLocalTransform( rot );
    }

    void MeshFeatureTrackingComponent::setScale( Scalar scale )
    {
        auto T = m_RO->getLocalTransform();
        m_RO->setLocalTransform( T.scale( scale ) );
    }

    void MeshFeatureTrackingComponent::setData( const Ra::Gui::FeatureData& data )
    {
        m_data = data;
    }

    void MeshFeatureTrackingComponent::update()
    {
        setPosition( getFeaturePosition() );
        setScale( getFeatureScale() );
        m_RO->setVisible( m_data.m_featureType != Ra::Engine::Renderer::RO );
    }

    Scalar MeshFeatureTrackingComponent::getFeatureScale() const
    {
        if (m_data.m_featureType == Ra::Engine::Renderer::RO)
        {
            return 1.0;
        }
        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_data.m_roIdx);
        const auto& V = ro->getMesh()->getGeometry().m_vertices;
        switch (m_data.m_featureType)
        {
        case Ra::Engine::Renderer::VERTEX:
        {
            // return 1 fourth of the edge length of the first edge we can find with the vertex
            const auto& T = ro->getMesh()->getGeometry().m_triangles;
            const auto& v = m_data.m_data[0];
            for (const auto& t : T)
            {
                if (t(0) == v || t(1) == v || t(2) == v)
                {
                    const Ra::Core::Vector3& v0 = V[ v ];
                    const Ra::Core::Vector3& v1 = V[ t(0)==v? t(1) : t(0) ];
                    return (v1-v0).norm() / 4.0;
                }
            }
            return ro->getAabb().diagonal().norm() / 100; // default for point clouds
        }
        case Ra::Engine::Renderer::EDGE:
        {
            // return 1 fourth of the edge length
            const Ra::Core::Vector3& v0 = V[ m_data.m_data[0] ];
            const Ra::Core::Vector3& v1 = V[ m_data.m_data[1] ];
            return (v1-v0).norm() / 4.0;
        }
        case Ra::Engine::Renderer::TRIANGLE:
        {
            // return half the smallest distance from C to an edge
            const Ra::Core::Vector3& v0 = V[ m_data.m_data[1] ];
            const Ra::Core::Vector3& v1 = V[ m_data.m_data[2] ];
            const Ra::Core::Vector3& v2 = V[ m_data.m_data[3] ];
            const Ra::Core::Vector3 C = ( v0 + v1 + v2 ) / 3.0;
            const Ra::Core::Vector3 C0 = C-v0;
            const Ra::Core::Vector3 C1 = C-v1;
            const Ra::Core::Vector3 C2 = C-v2;
            return sqrt( std::min(std::min( C0.squaredNorm() * (v1-v0).normalized().cross(C0.normalized()).squaredNorm(),
                                            C1.squaredNorm() * (v2-v1).normalized().cross(C1.normalized()).squaredNorm()),
                                            C2.squaredNorm() * (v0-v2).normalized().cross(C2.normalized()).squaredNorm()) ) / 2.0;
        }
        default:
            return ro->getAabb().diagonal().norm() / 100;
        }
    }

    Ra::Core::Vector3 MeshFeatureTrackingComponent::getFeaturePosition() const
    {
        if (m_data.m_featureType == Ra::Engine::Renderer::RO)
        {
            return Ra::Core::Vector3();
        }

        const auto& v = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_data.m_roIdx)
                                                               ->getMesh()->getGeometry().m_vertices;
        Ra::Core::Vector3 P(0,0,0);
        switch (m_data.m_featureType)
        {
        case Ra::Engine::Renderer::VERTEX:
        {
            P = v[ m_data.m_data[0] ];
            break;
        }
        case Ra::Engine::Renderer::EDGE:
        {
            P = ( v[ m_data.m_data[0] ]
                + v[ m_data.m_data[1] ] ) / 2.0;
            break;
        }
        case Ra::Engine::Renderer::TRIANGLE:
        {
            P = ( v[ m_data.m_data[1] ]
                + v[ m_data.m_data[2] ]
                + v[ m_data.m_data[3] ] ) / 3.0;
            break;
        }
        default:
            break;
        }

        return P;
    }

    Ra::Core::Vector3 MeshFeatureTrackingComponent::getFeatureVector() const
    {
        if (m_data.m_featureType == Ra::Engine::Renderer::RO)
        {
            return Ra::Core::Vector3();
        }

        const auto& v = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_data.m_roIdx)
                                                               ->getMesh()->getGeometry().m_vertices;
        const auto& n = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_data.m_roIdx)
                                                               ->getMesh()->getGeometry().m_normals;
        Ra::Core::Vector3 V(0,0,0);
        switch (m_data.m_featureType)
        {
        case Ra::Engine::Renderer::VERTEX:
        {
            // for vertices, the normal
            V = n[ m_data.m_data[0] ];
            break;
        }
        case Ra::Engine::Renderer::EDGE:
        {
            // for edges, the edge vector
            V = v[ m_data.m_data[0] ] - v[ m_data.m_data[1] ];
            break;
        }
        case Ra::Engine::Renderer::TRIANGLE:
        {
            // for triangles, the normal
            const Ra::Core::Vector3& p0 = v[ m_data.m_data[1] ];
            const Ra::Core::Vector3& p1 = v[ m_data.m_data[2] ];
            const Ra::Core::Vector3& p2 = v[ m_data.m_data[3] ];
            V = (p1-p0).cross(p2-p0).normalized();
            break;
        }
        default:
            break;
        }

        return V;
    }

}
