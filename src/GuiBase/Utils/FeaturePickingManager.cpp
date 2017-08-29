#include <GuiBase/Utils/FeaturePickingManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Core/Containers/MakeShared.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra
{
    namespace Gui
    {

        FeaturePickingManager::FeaturePickingManager() :
            m_firstRO(0),
            m_sphereComponent(nullptr)
        {
            Ra::Engine::RadiumEngine* engine = Ra::Engine::RadiumEngine::getInstance();
            Ra::Engine::Entity* e = engine->getEntityManager()->createEntity("Sphere");
            m_sphereComponent = new SphereComponent;
            e->addComponent(m_sphereComponent);
            m_sphereComponent->initialize();
            m_sphereComponent->getSphereRo()->setVisible(false);
            m_sphereComponent->getSphereRo()->setPickable(false);
        }

        FeaturePickingManager::~FeaturePickingManager()
        {
            delete m_sphereComponent;
        }

        void FeaturePickingManager::doPicking( int roIndex, const Engine::Renderer::PickingQuery &query, const Core::Ray& ray )
        {
            // first clear the feature data.
            m_FeatureData.m_featureType = query.m_mode;
            m_FeatureData.m_data.clear();
            m_FeatureData.m_roIdx = roIndex;
            if (roIndex < m_firstRO || roIndex == -1)
            {
                m_FeatureData.m_featureType = Engine::Renderer::RO;
            }
            // if picking is on the RO, the nothing to be done here
            if (m_FeatureData.m_featureType == Engine::Renderer::RO)
            {
                return;
            }

            // pick triangle through raycasting
            auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(roIndex);
            const Ra::Core::Transform& t = ro->getLocalTransform();
            const Core::Ray transformedRay = Ra::Core::transformRay(ray, t.inverse());
            const auto result = Ra::Core::MeshUtils::castRay(ro->getMesh()->getGeometry(), transformedRay);
            const int& tidx = result.m_hitTriangle;

            // fill feature data
            if (tidx<0) // didn't select any
            {
                m_FeatureData.m_featureType = Engine::Renderer::RO;
                return;
            }
            switch (query.m_mode)
            {
            case Engine::Renderer::VERTEX:
            {
                // data is the vertex index
                m_FeatureData.m_data.push_back( result.m_nearestVertex );
                break;
            }
            case Engine::Renderer::EDGE:
            {
                // data are the edge vertices indices
                // FIXME: some issues there
                m_FeatureData.m_data.push_back( result.m_edgeVertex0 );
                m_FeatureData.m_data.push_back( result.m_edgeVertex1 );
                break;
            }
            case Engine::Renderer::TRIANGLE:
            {
                // data is the triangle index, along with its vertices indices
                m_FeatureData.m_data.push_back( tidx );
                const auto& T = ro->getMesh()->getGeometry().m_triangles[ tidx ];
                m_FeatureData.m_data.push_back( T(0) );
                m_FeatureData.m_data.push_back( T(1) );
                m_FeatureData.m_data.push_back( T(2) );
                break;
            }
            default:
                break;
            }
        }

        void FeaturePickingManager::clearFeature()
        {
            m_FeatureData.m_featureType = Engine::Renderer::RO;
            m_FeatureData.m_data.clear();
            m_FeatureData.m_roIdx = -1;
            m_sphereComponent->getSphereRo()->setVisible(false);
        }

        void FeaturePickingManager::setVertexIndex(int id)
        {
            if (m_FeatureData.m_featureType == Engine::Renderer::VERTEX)
            {
                auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_FeatureData.m_roIdx);
                if (id < ro->getMesh()->getGeometry().m_vertices.size())
                {
                    m_FeatureData.m_data[0] = id;
                }
            }
        }

        void FeaturePickingManager::setTriangleIndex(int id)
        {
            if (m_FeatureData.m_featureType == Engine::Renderer::TRIANGLE)
            {
                m_FeatureData.m_data[0] = id;
                auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_FeatureData.m_roIdx);
                if (id < ro->getMesh()->getGeometry().m_triangles.size())
                {
                    const auto& T = ro->getMesh()->getGeometry().m_triangles[id];
                    m_FeatureData.m_data[1] = T(0);
                    m_FeatureData.m_data[2] = T(1);
                    m_FeatureData.m_data[3] = T(2);
                }
            }
        }

        void FeaturePickingManager::setSpherePosition()
        {
            auto sphereRoName = m_sphereComponent -> getSphereRo() -> getName();

            if( m_sphereComponent )
            {
                m_sphereComponent->setPosition( getFeaturePosition() );
                m_sphereComponent->setScale( getScaleFromFeature() );
            }
            m_sphereComponent->getSphereRo()->setVisible( m_FeatureData.m_featureType != Engine::Renderer::RO );
        }

        Scalar FeaturePickingManager::getScaleFromFeature() const
        {
            if (m_FeatureData.m_featureType == Engine::Renderer::RO)
            {
                return 1.0;
            }
            auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_FeatureData.m_roIdx);
            const auto& V = ro->getMesh()->getGeometry().m_vertices;
            switch (m_FeatureData.m_featureType)
            {
            case Engine::Renderer::VERTEX:
            {
                // return 1 fourth of the edge length of the first edge we can find with the vertex
                const auto& T = ro->getMesh()->getGeometry().m_triangles;
                const auto& v = m_FeatureData.m_data[0];
                for (const auto& t : T)
                {
                    if (t(0) == v || t(1) == v || t(2) == v)
                    {
                        const Core::Vector3& v0 = V[ v ];
                        const Core::Vector3& v1 = V[ t(0)==v? t(1) : t(0) ];
                        return (v1-v0).norm() / 4.0;
                    }
                }
                return 1.0; // should never come here
            }
            case Engine::Renderer::EDGE:
            {
                // return 1 fourth of the edge length
                const Core::Vector3& v0 = V[ m_FeatureData.m_data[0] ];
                const Core::Vector3& v1 = V[ m_FeatureData.m_data[1] ];
                return (v1-v0).norm() / 4.0;
            }
            case Engine::Renderer::TRIANGLE:
            {
                // return half the smallest distance from C to an edge
                const Core::Vector3& v0 = V[ m_FeatureData.m_data[1] ];
                const Core::Vector3& v1 = V[ m_FeatureData.m_data[2] ];
                const Core::Vector3& v2 = V[ m_FeatureData.m_data[3] ];
                const Core::Vector3 C = ( v0 + v1 + v2 ) / 3.0;
                const Core::Vector3 C0 = C-v0;
                const Core::Vector3 C1 = C-v1;
                const Core::Vector3 C2 = C-v2;
                return sqrt( std::min(std::min( C0.squaredNorm() * (v1-v0).normalized().cross(C0.normalized()).squaredNorm(),
                                                C1.squaredNorm() * (v2-v1).normalized().cross(C1.normalized()).squaredNorm()),
                                                C2.squaredNorm() * (v0-v2).normalized().cross(C2.normalized()).squaredNorm()) ) / 2.0;
            }
            default:
                return 1.0;
            }
        }

        //GET Vertex Information

        Core::Vector3 FeaturePickingManager::getFeaturePosition() const
        {
            if (m_FeatureData.m_featureType == Engine::Renderer::RO)
            {
                return Core::Vector3();
            }

            const auto& v = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_FeatureData.m_roIdx)
                                                                   ->getMesh()->getGeometry().m_vertices;
            Core::Vector3 P(0,0,0);
            switch (m_FeatureData.m_featureType)
            {
            case Engine::Renderer::VERTEX:
            {
                P = v[ m_FeatureData.m_data[0] ];
                break;
            }
            case Engine::Renderer::EDGE:
            {
                P = ( v[ m_FeatureData.m_data[0] ]
                    + v[ m_FeatureData.m_data[1] ] ) / 2.0;
                break;
            }
            case Engine::Renderer::TRIANGLE:
            {
                P = ( v[ m_FeatureData.m_data[1] ]
                    + v[ m_FeatureData.m_data[2] ]
                    + v[ m_FeatureData.m_data[3] ] ) / 3.0;
                break;
            }
            default:
                break;
            }

            return P;
        }


        Core::Vector3 FeaturePickingManager::getFeatureVector() const
        {
            if (m_FeatureData.m_featureType == Engine::Renderer::RO)
            {
                return Core::Vector3();
            }

            const auto& v = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_FeatureData.m_roIdx)
                                                                   ->getMesh()->getGeometry().m_vertices;
            const auto& n = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(m_FeatureData.m_roIdx)
                                                                   ->getMesh()->getGeometry().m_normals;
            Core::Vector3 V(0,0,0);
            switch (m_FeatureData.m_featureType)
            {
            case Engine::Renderer::VERTEX:
            {
                // for vertices, the normal
                V = n[ m_FeatureData.m_data[0] ];
                break;
            }
            case Engine::Renderer::EDGE:
            {
                // for edges, the edge vector
                V = v[ m_FeatureData.m_data[0] ] - v[ m_FeatureData.m_data[1] ];
                break;
            }
            case Engine::Renderer::TRIANGLE:
            {
                // for triangles, the normal
                const Core::Vector3& p0 = v[ m_FeatureData.m_data[1] ];
                const Core::Vector3& p1 = v[ m_FeatureData.m_data[2] ];
                const Core::Vector3& p2 = v[ m_FeatureData.m_data[3] ];
                V = (p1-p0).cross(p2-p0).normalized();
                break;
            }
            default:
                break;
            }

            return V;
        }



        SphereComponent::SphereComponent()
                : Ra::Engine::Component("SphereC Component"), m_sphereRo (nullptr)
        {
            m_sphere = Ra::Core::MeshUtils::makeParametricSphere(1.0);
        }


        void SphereComponent::initialize()
        {
            std::shared_ptr<Ra::Engine::Mesh> display(new Ra::Engine::Mesh("FeaturePickingManagerSphere"));
            display->loadGeometry(m_sphere);
            std::shared_ptr<Ra::Engine::Material> material;
            material.reset( new Ra::Engine::Material("VertexPickingManageSphereMaterial") );
            material-> m_kd = Ra::Core::Color(1.f,0.f,0.f,1.f);
            m_sphereRo = Ra::Engine::RenderObject::createRenderObject("FeaturePickingManagerSphereRO",
                                                                      this,
                                                                      Ra::Engine::RenderObjectType::Fancy,
                                                                      display,
                                                                      Ra::Engine::ShaderConfigurationFactory::getConfiguration("BlinnPhong"),
                                                                      material);
            addRenderObject(m_sphereRo);
        }


        void SphereComponent::setPosition (Ra::Core::Vector3 position)
        {
            if (m_sphereRo)
            {
                Ra::Core::Translation aa(position);
                Ra::Core::Transform rot(aa);
                m_sphereRo->setLocalTransform( rot );
            }
        }
        void SphereComponent::setScale (Scalar scale)
        {
            if (m_sphereRo)
            {
                auto T = m_sphereRo->getLocalTransform();
                m_sphereRo->setLocalTransform( T.scale( scale ) );
            }
        }

        Engine::RenderObject* SphereComponent::getSphereRo ()
        {
            return m_sphereRo;
        }
    }
}

