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

//        void MeshContactElement::addMesh(
//        ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite verticesWriter,
//        ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite normalsWriter,
//        ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite trianglesWriter
//        )
//        {
//            m_verticesWriter = verticesWriter;
//            m_normalsWriter = normalsWriter;
//            m_trianglesWriter = trianglesWriter;
//        }

//        void MeshContactElement::setlodValueChanged(int value)
//        {
//            // retrieving the data of the displayed mesh
//            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
//            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
//            TriangleArray& triangles = *(m_trianglesWriter());

//            // go to LOD with 'value' number of faces
//            Ra::Core::TriangleMesh mNew = m_pmlod->gotoM(value);

//            // update of the data of the displayed mesh
//            triangles = mNew.m_triangles;
//            vertices = mNew.m_vertices;
//            Ra::Core::Geometry::uniformNormal(vertices, triangles, normals);
//        }

        int MeshContactElement::getNbFacesMax()
        {
//            TriangleArray& triangles = *(m_trianglesWriter());
//            return triangles.size();
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

//        void MeshContactElement::computeTriangleMesh() //use Mesh instead of Triangle Mesh
//        {
//            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
//            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
//            TriangleArray& triangles = *(m_trianglesWriter());

//            Ra::Core::TriangleMesh* m = new Ra::Core::TriangleMesh();
//            m->m_vertices = vertices;
//            m->m_normals = normals;
//            m->m_triangles = triangles;

//            m_initTriangleMesh = *m;
//        }

        void MeshContactElement::computeMesh(const std::string& entityName, const std::string& componentName)
        {
            RadiumEngine* engine = RadiumEngine::getInstance();
            m_mesh = engine->getMesh(entityName, componentName);

//            Ra::Core::TriangleMesh tm = Ra::Core::TriangleMesh(engine->getMesh(entityName,componentName)->getGeometry());
//            m_mesh = new Ra::Engine::Mesh(entityName);
//            m_mesh->loadGeometry(tm);

            m_initTriangleMesh = m_mesh->getGeometry();
        }

//        void MeshContactElement::computeProgressiveMesh()
//        {
//            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
//            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
//            TriangleArray& triangles = *(m_trianglesWriter());

//            Ra::Core::TriangleMesh* m = new Ra::Core::TriangleMesh();
//            m->m_vertices = vertices;
//            m->m_normals = normals;
//            m->m_triangles = triangles;

//            m_initTriangleMesh = *m;

//            Ra::Core::ProgressiveMeshBase<>* pm = new Ra::Core::ProgressiveMesh<>(m);
//            m_pmlod = new Ra::Core::ProgressiveMeshLOD(pm);
//        }

        Ra::Core::TriangleMesh MeshContactElement::getInitTriangleMesh()
        {
            return m_initTriangleMesh;
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
                LOG(logINFO) << "Collapse not possible";
                getPriorityQueue()->top();
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
//            Ra::Core::Vector3Array& vertices = *(m_verticesWriter());
//            Ra::Core::Vector3Array& normals = *(m_normalsWriter());
//            TriangleArray& triangles = *(m_trianglesWriter());

//            vertices = newMesh.m_vertices;
//            triangles = newMesh.m_triangles;
//            Ra::Core::Geometry::uniformNormal(vertices, triangles, normals);
            m_mesh->loadGeometry(newMesh);
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
