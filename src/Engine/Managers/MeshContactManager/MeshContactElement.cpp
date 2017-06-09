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
#include <Engine/Assets/GeometryData.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <Core/TreeStructures/kdtree.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

namespace Ra
{
    namespace Engine
    {

        void MeshContactElement::addMesh(
        ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite verticesWriter,
        ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite normalsWriter,
        ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite trianglesWriter
        )
        {
            m_verticesWriter = verticesWriter;
            m_normalsWriter = normalsWriter;
            m_trianglesWriter = trianglesWriter;
        }

        void MeshContactElement::setlodValueChanged(int value)
        {
            // Retrieving the data of the displayed mesh
            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
            TriangleArray& triangles = *(m_trianglesWriter());

            // Go to LOD with 'value' number of faces
            Ra::Core::TriangleMesh mNew = m_pmlod->gotoM(value);

            // Update of the data of the displayed mesh
            triangles = mNew.m_triangles;
            vertices = mNew.m_vertices;
            Ra::Core::Geometry::uniformNormal(vertices, triangles, normals);
        }

        int MeshContactElement::getNbFacesMax()
        {
            TriangleArray& triangles = *(m_trianglesWriter());
            return triangles.size();
        }


        Super4PCS::KdTree<Scalar>* MeshContactElement::computeKdTree()
        {
            const Super4PCS::KdTree<Scalar>::PointList& points = reinterpret_cast<const Super4PCS::KdTree<Scalar>::PointList&>(*(m_verticesWriter()));
            return (new Super4PCS::KdTree<Scalar>(points));
        }

        Ra::Core::PriorityQueue* MeshContactElement::getPriorityQueue()
        {
            return m_pqueue;
        }

        void MeshContactElement::setPriorityQueue(Ra::Core::PriorityQueue pQueue)
        {
            m_pqueue = new Ra::Core::PriorityQueue(pQueue);
        }

        void MeshContactElement::computeProgressiveMesh()
        {
            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
            TriangleArray& triangles = *(m_trianglesWriter());

            Ra::Core::TriangleMesh* m = new Ra::Core::TriangleMesh();
            m->m_vertices = vertices;
            m->m_normals = normals;
            m->m_triangles = triangles;

            Ra::Core::ProgressiveMeshBase<>* pm = new Ra::Core::ProgressiveMesh<>(m);
            m_pmlod = new Ra::Core::ProgressiveMeshLOD(pm);
        }

        Ra::Core::ProgressiveMeshLOD* MeshContactElement::getProgressiveMeshLOD()
        {
            return m_pmlod;
        }

        bool MeshContactElement::isConstructM0()
        {
            Ra::Core::PriorityQueue::PriorityQueueData d = getPriorityQueue()->firstData();
            Ra::Core::HalfEdge_ptr he = getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_halfedge[d.m_edge_id];
            if (! getProgressiveMeshLOD()->getProgressiveMesh()->isEcolConsistent(he->idx, d.m_p_result))
            {
                LOG(logINFO) << "Collapse not possible";
                getPriorityQueue()->top(); //it's better not to delete the collapse from the priority queue, but to assign a very big error to it
                return false;
            }
            else if (! getProgressiveMeshLOD()->getProgressiveMesh()->isEcolPossible(he->idx, d.m_p_result))
            {
                LOG(logINFO) << "Collapse not possible";
                getPriorityQueue()->top();
                return false;
            }
            else
                return true;
        }

        void MeshContactElement::updateTriangleMesh(Ra::Core::TriangleMesh newMesh)
        {
            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
            TriangleArray& triangles = *(m_trianglesWriter());

            vertices = newMesh.m_vertices;
            triangles = newMesh.m_triangles;
            Ra::Core::Geometry::uniformNormal(vertices, triangles, normals);
        }

        void MeshContactElement::computePrimitives()
        {
            // We retrieve the initial faces quadrics
            std::vector<Ra::Core::ProgressiveMesh<>::Primitive> facesPrimitives = getProgressiveMeshLOD()->getProgressiveMesh()->getFacesQuadrics();

            int nbVertex = getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex.size();

            for (uint v = 0; v < nbVertex; v++)
            {
                // We go all over the faces which contain vertexIndex
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

         Ra::Core::ProgressiveMesh<>::Primitive MeshContactElement::getPrimitive(int vertexIndex)
         {
             return m_primitives[vertexIndex];
         }

         void MeshContactElement::setIndex(int idx)
         {
             m_index = idx;
         }

         int MeshContactElement::getIndex()
         {
             return m_index;
         }

    }
}
