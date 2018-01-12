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

        FeaturePickingManager::FeaturePickingManager() : m_firstRO(0)
        {}

        FeaturePickingManager::~FeaturePickingManager()
        {}

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
            if (query.m_mode != Engine::Renderer::VERTEX && tidx<0) // didn't select any
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

    }
}

