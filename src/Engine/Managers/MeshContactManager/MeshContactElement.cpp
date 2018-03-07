#include <Engine/Managers/MeshContactManager/MeshContactElement.hpp>

#include <queue>
#include <iostream>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/Wrapper/Convert.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
#include <Core/Geometry/Normal/Normal.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <Core/File/GeometryData.hpp>
#include <Core/TreeStructures/kdtree.hpp>
#include <Core/TreeStructures/trianglekdtree.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

#include <Engine/RadiumEngine.hpp>

namespace Ra
{
    namespace Engine
    {

        int MeshContactElement::getNbFacesMax()
        {
            return m_mesh->getGeometry().m_triangles.size();
        }

        Super4PCS::TriangleKdTree<>* MeshContactElement::computeTriangleKdTree(Ra::Core::TriangleMesh& tm)
        {
            const Super4PCS::TriangleKdTree<>::TriangleList& triangles = reinterpret_cast<const Super4PCS::TriangleKdTree<>::TriangleList&>(tm.m_triangles);
            const Super4PCS::TriangleKdTree<>::PointList& points = reinterpret_cast<const Super4PCS::TriangleKdTree<>::PointList&>(tm.m_vertices);
            return (new Super4PCS::TriangleKdTree<>(triangles, points));
        }

        Ra::Core::PriorityQueue* MeshContactElement::getPriorityQueue()
        {
            return m_pqueue;
        }

        void MeshContactElement::setPriorityQueue(Ra::Core::PriorityQueue pQueue)
        {
            m_pqueue = new Ra::Core::PriorityQueue(pQueue);
        }

        void MeshContactElement::computeMesh(const std::string& entityName, const std::string& componentName)
        {
            RadiumEngine* engine = RadiumEngine::getInstance();
            m_mesh = engine->getMesh(entityName, componentName);
            m_initTriangleMesh = m_mesh->getGeometry();
            setTriangleMeshDuplicate();
        }

        Ra::Core::TriangleMesh MeshContactElement::getInitTriangleMesh()
        {
            return m_initTriangleMesh;
        }

        void MeshContactElement::setTriangleMeshDuplicate()
        {
            m_tm_duplicateVertices.m_normals = m_initTriangleMesh.m_normals;

            int nbTriangles = m_initTriangleMesh.m_triangles.size();

            for (uint i = 0; i < nbTriangles; i++)
            {
                for (uint j = 0; j < 3; j++)
                {
                    m_tm_duplicateVertices.m_vertices.push_back(m_initTriangleMesh.m_vertices[m_initTriangleMesh.m_triangles[i][j]]);
                }

                Ra::Core::Triangle t;
                for (uint k = 0; k < 3; k++)
                {
                    t[k] = 3 * i + k;
                }

                m_tm_duplicateVertices.m_triangles.push_back(t);
            }
        }

        Ra::Core::TriangleMesh MeshContactElement::getTriangleMeshDuplicate()
        {
            return m_tm_duplicateVertices;
        }

        void MeshContactElement::setTriangleMeshDuplicateSimplified()
        {
            m_tm_duplicateVerticesSimplified.m_normals = m_mesh->getGeometry().m_normals;

            int nbTriangles = m_mesh->getGeometry().m_triangles.size();

            for (uint i = 0; i < nbTriangles; i++)
            {
                for (uint j = 0; j < 3; j++)
                {
                    m_tm_duplicateVerticesSimplified.m_vertices.push_back(m_mesh->getGeometry().m_vertices[m_mesh->getGeometry().m_triangles[i][j]]);
                }

                Ra::Core::Triangle t;
                for (uint k = 0; k < 3; k++)
                {
                    t[k] = 3 * i + k;
                }

                m_tm_duplicateVerticesSimplified.m_triangles.push_back(t);
            }
        }

        Ra::Core::TriangleMesh MeshContactElement::getTriangleMeshDuplicateSimplified()
        {
            return m_tm_duplicateVerticesSimplified;
        }

        Ra::Core::ProgressiveMeshLOD* MeshContactElement::getProgressiveMeshLOD()
        {
            return m_pmlod;
        }

        void MeshContactElement::setProgressiveMeshLOD(Ra::Core::ProgressiveMeshBase<>* pm)
        {
            m_pmlod = new Ra::Core::ProgressiveMeshLOD(pm);
        }

        bool MeshContactElement::isConstructM0()
        {
            Ra::Core::PriorityQueue::PriorityQueueData d = getPriorityQueue()->firstData();
            Ra::Core::HalfEdge_ptr he = getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_halfedge[d.m_edge_id];
            if (! getProgressiveMeshLOD()->getProgressiveMesh()->isEcolConsistent(he->idx, d.m_p_result))
            {
                getPriorityQueue()->top();
                return false;
            }
            else if (! getProgressiveMeshLOD()->getProgressiveMesh()->isEcolPossible(he->idx, d.m_p_result))
            {
                getPriorityQueue()->top();
                return false;
            }
            else
                return true;
        }

        void MeshContactElement::updateTriangleMesh(Ra::Core::TriangleMesh newMesh)
        {
            m_mesh->loadGeometry(newMesh);

            Ra::Core::Vector4 vertexColor (0, 0, 0, 0);
            int nbVertices = m_mesh->getGeometry().m_vertices.size();
            Ra::Core::Vector4Array colors;
            for (uint v = 0; v < nbVertices; v++)
            {
                colors.push_back(vertexColor);
            }
            m_mesh->addData(Ra::Engine::Mesh::VERTEX_COLOR, colors);

            setTriangleMeshDuplicateSimplified();
        }

        void MeshContactElement::computePrimitives()
        {
            // we retrieve the initial faces quadrics
            std::vector<Ra::Core::ProgressiveMesh<>::Primitive> facesPrimitives = getProgressiveMeshLOD()->getProgressiveMesh()->getFacesQuadrics();

            int nbVertex = getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex.size();

            for (uint v = 0; v < nbVertex; v++)
            {
                // we go all over the faces which contain vertexIndex
                Ra::Core::VFIterator vfIt = Ra::Core::VFIterator(getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[v]);
                Ra::Core::FaceList adjFaces = vfIt.list();

                Ra::Core::ProgressiveMesh<>::Primitive q = facesPrimitives[adjFaces[0]->idx];
                for (uint i = 1; i < adjFaces.size(); i++)
                {
                    q += facesPrimitives[adjFaces[i]->idx];
                }

                Scalar weight = 1.0/adjFaces.size();
                q *= weight;

                m_primitives.push_back(q);
            }
         }

        void MeshContactElement::computeFacePrimitives()
        {
            m_faceprimitives = getProgressiveMeshLOD()->getProgressiveMesh()->getFacesQuadrics();
         }

         Ra::Core::ProgressiveMesh<>::Primitive MeshContactElement::getPrimitive(int vertexIndex)
         {
             return m_primitives[vertexIndex];
         }

         Ra::Core::ProgressiveMesh<>::Primitive MeshContactElement::getFacePrimitive(int faceIndex)
         {
             return m_faceprimitives[faceIndex];
         }

         void MeshContactElement::setIndex(int idx)
         {
             m_index = idx;
         }

         int MeshContactElement::getIndex()
         {
             return m_index;
         }

         Mesh* MeshContactElement::getMesh()
         {
             return m_mesh;
         }

         void MeshContactElement::setMesh(const Ra::Core::TriangleMesh& tm)
         {
             m_mesh->loadGeometry(tm);
         }

    }
}
