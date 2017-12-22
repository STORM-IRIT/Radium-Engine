#include <MeshFeatureTrackingComponent.hpp>

#include <queue>
#include <iostream>

#include <Core/Mesh/TriangleMesh.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Material/BlinnPhongMaterial.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <queue>
#include <iostream>

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
        std::shared_ptr<Ra::Engine::Mesh> display( new Ra::Engine::Mesh("PickingManagerSphere") );
        auto sphere = Ra::Core::MeshUtils::makeParametricSphere(1.0);
        display->loadGeometry( sphere );
        std::shared_ptr<Ra::Engine::Material> material;
        auto bpMaterial = new Ra::Engine::BlinnPhongMaterial("PickingManageSphereMaterial");
        material.reset( bpMaterial);
        bpMaterial->m_kd = Ra::Core::Color(0.f,1.f,0.f,1.f);
        m_RO = Ra::Engine::RenderObject::createRenderObject( "FeaturePickingManagerSphereRO", this,
                                                             Ra::Engine::RenderObjectType::Fancy, display,
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

    void MeshFeatureTrackingComponent::setData( const Ra::Engine::Renderer::PickingResult &data )
    {
        m_data = data;
    }

    const Ra::Engine::Renderer::PickingResult& MeshFeatureTrackingComponent::getData() const
    {
        return m_data;
    }

    int MeshFeatureTrackingComponent::getMaxV() const
    {
        if ( m_data.m_mode != Ra::Engine::Renderer::RO &&
             m_data.m_mode < Ra::Engine::Renderer::C_VERTEX &&
             getRoMgr()->exists( m_data.m_roIdx ) )
        {
            return Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx )
                    ->getMesh()->getGeometry().m_vertices.size();
        }
        return 0;
    }

    int MeshFeatureTrackingComponent::getMaxT() const
    {
        if ( m_data.m_mode != Ra::Engine::Renderer::RO &&
             m_data.m_mode < Ra::Engine::Renderer::C_VERTEX &&
             getRoMgr()->exists( m_data.m_roIdx ) )
        {
            auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx );
            if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_TRIANGLES)
            {
                return ro->getMesh()->getGeometry().m_triangles.size();
            }
        }
        return 0;
    }

    void MeshFeatureTrackingComponent::setVertexIdx( int idx )
    {
        if ( getRoMgr()->exists( m_data.m_roIdx ) )
        {
            auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx );
            if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS)
            {
                m_data.m_elementIdx[0] = idx;
                return;
            }
            if (ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_TRIANGLES)
            {
                return;
            }
            const auto& T = ro->getMesh()->getGeometry().m_triangles;
            for ( int t = 0; t < T.size(); ++t )
            {
                if (T[t](0) == idx)
                {
                    m_data.m_elementIdx[0] = t;
                    m_data.m_vertexIdx[0] = 0;
                    return;
                }
                if (T[t](1) == idx)
                {
                    m_data.m_elementIdx[0] = t;
                    m_data.m_vertexIdx[0] = 1;
                    return;
                }
                if (T[t](2) == idx)
                {
                    m_data.m_elementIdx[0] = t;
                    m_data.m_vertexIdx[0] = 2;
                    return;
                }
            }
        };
    }

    void MeshFeatureTrackingComponent::setTriangleIdx( int idx )
    {
        m_data.m_elementIdx[0] = idx;
    }

    void MeshFeatureTrackingComponent::update()
    {
        // check supported picking mode
        if ( m_data.m_mode != Ra::Engine::Renderer::RO &&
             m_data.m_mode < Ra::Engine::Renderer::C_VERTEX &&
             getRoMgr()->exists( m_data.m_roIdx ) )
        {
        	setPosition( getFeaturePosition() );
        	setScale( getFeatureScale() );
            auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx );
            m_RO->setVisible( ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_POINTS ||
                              m_data.m_mode == Ra::Engine::Renderer::VERTEX );
        }
        else
        {
            m_RO->setVisible( false );
        }
    }

    FeatureData MeshFeatureTrackingComponent::getFeatureData() const
    {
        FeatureData data;
        data.m_mode = Ra::Engine::Renderer::RO;
        // check supported picking mode
        if ( m_data.m_mode == Ra::Engine::Renderer::RO ||
             m_data.m_mode >= Ra::Engine::Renderer::C_VERTEX ||
             !getRoMgr()->exists( m_data.m_roIdx ) )
        {
            return data;
        }

        // check supported RO type
        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx );
        if ( ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_POINTS &&
             ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_TRIANGLES )
        {
            return data;
        }

        // manage picking mode
        if ( m_data.m_mode == Ra::Engine::Renderer::VERTEX)
        {
            data.m_mode = m_data.m_mode;
            if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS)
            {
                data.m_data[0] = m_data.m_elementIdx[0];
            }
            else
            {
                data.m_data[0] = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ]( m_data.m_vertexIdx[0] );
            }
        }
        if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS)
        {
            return data;
        }
        if ( m_data.m_mode == Ra::Engine::Renderer::EDGE )
        {
            data.m_mode = m_data.m_mode;
            data.m_data[0] = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ]( (m_data.m_edgeIdx[0]+1)%3 );
            data.m_data[1] = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ]( (m_data.m_edgeIdx[0]+2)%3 );
        }
        if ( m_data.m_mode == Ra::Engine::Renderer::TRIANGLE )
        {
            data.m_mode = m_data.m_mode;
            data.m_data[0] = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ]( 0 );
            data.m_data[1] = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ]( 1 );
            data.m_data[2] = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ]( 2 );
            data.m_data[3] = m_data.m_elementIdx[0];
        }
        return data;
    }

    Scalar MeshFeatureTrackingComponent::getFeatureScale() const
    {
        // check supported picking mode
        if ( m_data.m_mode == Ra::Engine::Renderer::RO ||
             m_data.m_mode >= Ra::Engine::Renderer::C_VERTEX ||
             !getRoMgr()->exists( m_data.m_roIdx ) )
        {
            return 1.0;
        }

        // check supported RO type
        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_data.m_roIdx);
        if ( ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_POINTS &&
             ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_TRIANGLES )
        {
            return 1.0;
        }
        if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS)
        {
            return ro->getAabb().sizes().norm() / 500;
        }

        // manage picking mode
        const auto& v = ro->getMesh()->getGeometry().m_vertices;
        const auto& T = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ];
        switch (m_data.m_mode)
        {
        case Ra::Engine::Renderer::VERTEX:
        {
            // return 1 fourth of the edge length of the first edge we can find with the vertex
            const auto& V = T( m_data.m_vertexIdx[0] );
            for (const auto& t : ro->getMesh()->getGeometry().m_triangles)
            {
                if (t(0) == V || t(1) == V || t(2) == V)
                {
                    const Ra::Core::Vector3& v0 = v[ V ];
                    const Ra::Core::Vector3& v1 = v[ t(0)==V? t(1) : t(0) ];
                    return (v1-v0).norm() / 4.0;
                }
            }
            return ro->getAabb().diagonal().norm() / 100; // default for point clouds
        }
        case Ra::Engine::Renderer::EDGE:
        {
            // return 1 fourth of the edge length
            const Ra::Core::Vector3& v0 = v[ T( (m_data.m_edgeIdx[0]+1)%3 ) ];
            const Ra::Core::Vector3& v1 = v[ T( (m_data.m_edgeIdx[0]+2)%3 ) ];
            return (v1-v0).norm() / 4.0;
        }
        case Ra::Engine::Renderer::TRIANGLE:
        {
            // return half the smallest distance from C to an edge
            const Ra::Core::Vector3& v0 = v[ T(0) ];
            const Ra::Core::Vector3& v1 = v[ T(1) ];
            const Ra::Core::Vector3& v2 = v[ T(2) ];
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
        // check supported picking mode
        if ( m_data.m_mode == Ra::Engine::Renderer::RO ||
             m_data.m_mode >= Ra::Engine::Renderer::C_VERTEX ||
             !getRoMgr()->exists( m_data.m_roIdx ) )
        {
            return Ra::Core::Vector3();
        }

        // check supported RO type
        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx );
        if ( ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_POINTS &&
             ro->getMesh()->getRenderMode() != Ra::Engine::Mesh::RM_TRIANGLES )
        {
            return Ra::Core::Vector3();
        }
        const auto& v = ro->getMesh()->getGeometry().m_vertices;
        if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS)
        {
            return v[ m_data.m_elementIdx[0] ];
        }

        // manage picking mode
        const auto& T = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ];
        Ra::Core::Vector3 P(0,0,0);
        switch (m_data.m_mode)
        {
        case Ra::Engine::Renderer::VERTEX:
        {
            P = v[ T( m_data.m_vertexIdx[0] ) ];
            break;
        }
        case Ra::Engine::Renderer::EDGE:
        {
            P = ( v[ T( (m_data.m_edgeIdx[0]+1)%3 ) ] + v[ T( (m_data.m_edgeIdx[0]+2)%3 ) ] ) / 2.0;
            break;
        }
        case Ra::Engine::Renderer::TRIANGLE:
        {
            P = ( v[ T(0) ] + v[ T(1) ] + v[ T(2) ] ) / 3.0;
            break;
        }
        default:
            break;
        }

        return P;
    }

    Ra::Core::Vector3 MeshFeatureTrackingComponent::getFeatureVector() const
    {
        // check supported picking mode
        if ( m_data.m_mode == Ra::Engine::Renderer::RO ||
             m_data.m_mode >= Ra::Engine::Renderer::C_VERTEX ||
             !getRoMgr()->exists( m_data.m_roIdx ) )
        {
            return Ra::Core::Vector3();
        }

        // check supported RO type
        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_data.m_roIdx );
        const auto& n = ro->getMesh()->getGeometry().m_normals;
        if (ro->getMesh()->getRenderMode() == Ra::Engine::Mesh::RM_POINTS)
        {
            if ( !n.empty() )
            {
                return n[ m_data.m_elementIdx[0] ];
            }
            return Ra::Core::Vector3();
        }

        // manage picking mode
        const auto& v = ro->getMesh()->getGeometry().m_vertices;
        const auto& T = ro->getMesh()->getGeometry().m_triangles[ m_data.m_elementIdx[0] ];
        Ra::Core::Vector3 V(0,0,0);
        switch (m_data.m_mode)
        {
        case Ra::Engine::Renderer::VERTEX:
        {
            // for vertices, the normal
            V = n[ T( m_data.m_vertexIdx[0] ) ];
            break;
        }
        case Ra::Engine::Renderer::EDGE:
        {
            // for edges, the edge vector
            V = v[ T( (m_data.m_edgeIdx[0]+1)%3 ) ] - v[ T( (m_data.m_edgeIdx[0]+2)%3 ) ];
            break;
        }
        case Ra::Engine::Renderer::TRIANGLE:
        {
            // for triangles, the normal
            const Ra::Core::Vector3& p0 = v[ T(0) ];
            const Ra::Core::Vector3& p1 = v[ T(1) ];
            const Ra::Core::Vector3& p2 = v[ T(2) ];
            V = (p1-p0).cross(p2-p0).normalized();
            break;
        }
        default:
            break;
        }

        return V;
    }

}
