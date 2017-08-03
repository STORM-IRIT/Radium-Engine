#include <Engine/Managers/MeshContactManager/MeshContactManager.hpp>

#include <string>
#include <iostream>

#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Core/File/FileData.hpp>
#include <Core/File/HandleData.hpp>

#include <Engine/Entity/Entity.hpp>

#include "Eigen/Core"

#include <Core/Geometry/Normal/Normal.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>


namespace Ra
{
    namespace Engine
    {

        MeshContactManager::MeshContactManager()
            : m_nbfaces( 0 )
            ,m_threshold( 0.01 )
            ,m_lambda( 0.5 )
            ,m_nb_faces_max( 0 )
            ,m_curr_vsplit( 0 )
        {
        }

        void MeshContactManager::setNbFacesChanged(const int nb)
        {
            m_nbfaces = nb;
            computeNbFacesMax();
        }

        void MeshContactManager::computeNbFacesMax()
        {
            m_nb_faces_max = 0;

            for (const auto& elem : m_meshContactElements)
            {
                m_nb_faces_max += static_cast<MeshContactElement*>(elem)->getNbFacesMax();
            }
        }

        void MeshContactManager::setThresholdChanged(const double threshold)
        {
            m_threshold = threshold;
        }

        void MeshContactManager::setLambdaChanged(const double lambda)
        {
            m_lambda = lambda;
        }

        void MeshContactManager::addMesh(MeshContactElement* mesh)
        {
            m_meshContactElements.push_back(mesh);
            m_kdtrees.push_back(mesh->computeKdTree());
            LOG(logINFO) << "m_kdtrees size : " << m_kdtrees.size();
            mesh->computeProgressiveMesh();
            mesh->getProgressiveMeshLOD()->getProgressiveMesh()->computeFacesQuadrics();
            mesh->computePrimitives();
            computeNbFacesMax();
        }

        void MeshContactManager::setLodValueChanged(int value)
        {
            if (m_nbfaces < value)
            {
                while (m_nbfaces < value)
                {
                    MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[m_index_pmdata[--m_curr_vsplit]]);
                    int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();
                    if (! obj->getProgressiveMeshLOD()->more())
                        break;
                    else
                    {
                        m_nbfaces += (obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces() - nbfaces);

                        //Find vs and vt in pmdata
    //                    int vsIndex = obj->getProgressiveMeshLOD()->getCurrentPMData().getVsId();
    //                    int vtIndex = obj->getProgressiveMeshLOD()->getCurrentPMData().getVtId();
    //                    obj->updateEllipsoidsVS(vsIndex,vtIndex);
                    }
                }
            }
            else if (m_nbfaces > value)
            {
                while (m_nbfaces > value)
                {
                    MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[m_index_pmdata[m_curr_vsplit++]]);
                    int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();
                    if (! obj->getProgressiveMeshLOD()->less())
                        break;
                    else
                    {
                        m_nbfaces -= (nbfaces - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces());

                        //Find vs and vt in pmdata
    //                    int vsIndex = obj->getProgressiveMeshLOD()->getCurrentPMData().getVsId();
    //                    int vtIndex = obj->getProgressiveMeshLOD()->getCurrentPMData().getVtId();
    //                    obj->updateEllipsoidsEC(vsIndex,vtIndex);
                    }
                }
            }

            for (const auto& elem : m_meshContactElements)
            {
                  MeshContactElement* obj = static_cast<MeshContactElement*>(elem);
                  Ra::Core::TriangleMesh newMesh;
                  Ra::Core::MeshConverter::convert(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh()), newMesh);
//                  Ra::Core::convertPM(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), newMesh);
                  obj->updateTriangleMesh(newMesh);

                  //obj->displayEllipsoids();

                  //add the display of the updated ellipsoids
            }
        }

        //simplification of the first loaded object only, the second one is there only to take into account contacts
        void MeshContactManager::setConstructM0()
        {
             constructPriorityQueues();


    //                  for (const auto& comp : m_components)
    //                  {
    //                        MeshContactComponent* obj = static_cast<MeshContactComponent*>(comp.second);
    //                        m_mainqueue.insert(obj->getPriorityQueue()->firstData());
    //                        LOG(logINFO) << "main queue size : " << m_mainqueue.size();
    //                  }

             MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[0]);
             m_mainqueue.insert(obj->getPriorityQueue()->firstData());


        // End criterion : number of faces set in the UI
                int i = 0;

                QueueContact::iterator it = m_mainqueue.begin();

                while (it != m_mainqueue.end() && m_nb_faces_max > m_nbfaces)
                {
                  const Ra::Core::PriorityQueue::PriorityQueueData &d = *it;
                  LOG(logINFO) << "Number of faces" << i << " : " << m_nb_faces_max;
                  MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[d.m_index]);
                  int nbfaces = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces();
                  int vs = d.m_vs.idx();
                  int vt = d.m_vt.idx();

                  //DEBUG for spinning top
                  if (vs == 190 || vt == 190)
                  {
                      LOG(logINFO) << "contact";
                  }

                  if (nbfaces > 2)
                  {
                    if (edgeCollapse(obj->getIndex()))
                    {
                      LOG(logINFO) << "Edge collapse of edge " << vs << " " << vt << " of object " << d.m_index << ", resulting vertex : (" << d.m_p_result(0,0) << ", " << d.m_p_result(1,0) << ", " << d.m_p_result(2,0) << ")";
                      m_index_pmdata.push_back(obj->getIndex());
                      m_curr_vsplit++;
                      m_nb_faces_max -= (nbfaces - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces());
                    }
                    LOG(logINFO) << "main queue size : " << m_mainqueue.size();
                    if (obj->getPriorityQueue()->size() > 0)
                    {
                        m_mainqueue.insert(obj->getPriorityQueue()->firstData());
                    }
                    else
                    {
                      LOG(logINFO) << "Priority queue empty";
                    }
                  }
                  LOG(logINFO) << "main queue size : " << m_mainqueue.size();
                  i++;
                  LOG(logINFO) << "i = " << i;
                  m_mainqueue.erase(it);
                  it = m_mainqueue.begin();
                }

              for (const auto& elem : m_meshContactElements)
              {
                    MeshContactElement* obj = static_cast<MeshContactElement*>(elem);

                    //switch from DCEL to mesh
                    Ra::Core::TriangleMesh newMesh;
                    Ra::Core::MeshConverter::convert(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh()), newMesh);
//                    Ra::Core::convertPM(*(obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), newMesh);

                    obj->updateTriangleMesh(newMesh);

    //                obj->computeQuadricDisplay();

    //                obj->displayEllipsoids();
              }

        }

        int MeshContactManager::getNbFacesMax()
        {
            return m_nb_faces_max;
        }

        void MeshContactManager::constructPriorityQueues()
        {
            for (uint objIndex=0; objIndex < /*m_components.size()*/1; objIndex++)
            {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);
            Ra::Core::PriorityQueue pQueue = Ra::Core::PriorityQueue();
//            const uint numTriangles = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh()->n_faces();
//            const uint numTriangles = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_face.size();


            //browse edges
            Scalar  edgeError;
            Ra::Core::Vector3 p = Ra::Core::Vector3::Zero();
            int j;
            Ra::Core::TopologicalMesh* topologicalMesh = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh();

            for(Ra::Core::TopologicalMesh::FaceIter f_it = topologicalMesh->faces_sbegin(); f_it != topologicalMesh->faces_end(); ++f_it)
            {
                const Ra::Core::TopologicalMesh::FaceHandle& f = *f_it;
                Ra::Core::TopologicalMesh::HalfedgeHandle h = topologicalMesh->halfedge_handle(f);
                for (j = 0; j < 3; j++)
                {
                    const Ra::Core::TopologicalMesh::VertexHandle& vs = topologicalMesh->from_vertex_handle(h);
                    const Ra::Core::TopologicalMesh::VertexHandle& vt = topologicalMesh->to_vertex_handle(h);

                    // To prevent adding twice the same edge
                    if (vs.idx() > vt.idx())
                    {
                        h = topologicalMesh->next_halfedge_handle(h);
                        continue;
                    }

                    if (vs.idx() == 190 || vt.idx() == 190)
                    {
                       LOG(logINFO) << "CONTACT";
                    }


                    // test if the edge can be collapsed or if it has contact
                    int vertexIndex = -1;
                    bool contact = false;
                    Ra::Core::ProgressiveMesh<>::Primitive qk;
                    Scalar dist;
                    Scalar weight;
                    Scalar sumWeight = 0;

                    // for each vertex, we look for all contacts with other objects and add the contact quadrics to the quadric of the vertex
                    Ra::Core::ProgressiveMesh<>::Primitive qc = Ra::Core::ProgressiveMesh<>::Primitive();
                    for (uint k=0; k<m_kdtrees.size(); k++)
                    {
                        if (k != objIndex)
                        {
                            MeshContactElement* otherObj = static_cast<MeshContactElement*>(m_meshContactElements[k]);
                            Ra::Core::TopologicalMesh* otherTopologicalMesh =otherObj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh();

                            vertexIndex = obj->getProgressiveMeshLOD()->getProgressiveMesh()->vertexContact(vs, m_kdtrees, k, m_threshold);
                            if ( vertexIndex != -1)
                            {
                                contact = true;
                                const Ra::Core::TopologicalMesh::VertexHandle& c = otherTopologicalMesh->vertex_handle(vertexIndex);
                                //dist = (c->P() - vs->P()).norm(); //in kdtree.hpp, the distance is a squared distance
                                dist = (Ra::Core::convertVec3OpenMeshToEigen(otherTopologicalMesh->point(c) - otherTopologicalMesh->point(vs))).squaredNorm();
                                weight = std::pow(std::pow(dist/m_threshold,2)-1,2);
                                //weight = 1;
                                sumWeight += weight;
                                qk = otherObj->getPrimitive(vertexIndex);
                                qk *= weight;
                                qc += qk;
                            }

                            vertexIndex = obj->getProgressiveMeshLOD()->getProgressiveMesh()->vertexContact(vt, m_kdtrees, k, m_threshold);
                            if ( vertexIndex != -1)
                            {
                                contact = true;
                                const Ra::Core::TopologicalMesh::VertexHandle& c = otherTopologicalMesh->vertex_handle(vertexIndex);
                                //dist = (c->P() - vt->P()).norm();
                                dist = (Ra::Core::convertVec3OpenMeshToEigen(otherTopologicalMesh->point(c) - otherTopologicalMesh->point(vt))).squaredNorm();
                                weight = std::pow(std::pow(dist/m_threshold,2)-1,2);
                                //weight = 1;
                                sumWeight += weight;
                                qk = otherObj->getPrimitive(vertexIndex);
                                qk *= weight;
                                qc += qk;
                            }
                        }
                    }

                    Ra::Core::ProgressiveMesh<>::Primitive qe = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeQuadric(h);
                    Ra::Core::ProgressiveMesh<>::Primitive q = Ra::Core::ProgressiveMesh<>::Primitive(qe);
                    if (contact)
                    {
                        Ra::Core::TopologicalMesh::VertexHandle vh_vs = topologicalMesh->from_vertex_handle(h);
                        Ra::Core::TopologicalMesh::VertexHandle vh_vt = topologicalMesh->to_vertex_handle(h);

                        std::vector<Ra::Core::TopologicalMesh::FaceHandle> facesAdj;
                        Ra::Core::TopologicalMesh::VertexFaceIter vf_it = topologicalMesh->vf_iter(vh_vs);
                        for(; vf_it.is_valid(); ++vf_it) {
                            facesAdj.push_back(vf_it);
                        }
                        vf_it = topologicalMesh->vf_iter(vh_vt);
                        for(; vf_it.is_valid(); ++vf_it) {
                            facesAdj.push_back(vf_it);
                        }

                        int nbFacesAdj = facesAdj.size();
                        q *= m_lambda * nbFacesAdj;
                        qc *= 1 - m_lambda;
                        q += qc;
                        q *= 1 / (m_lambda * nbFacesAdj + (1 - m_lambda) * sumWeight);
                    }

                    edgeError = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeErrorContact(h, p, q);

                    //insert into the priority queue with the real resulting point
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeErrorContact(h, p, qe);
                    pQueue.insert(Ra::Core::PriorityQueue::PriorityQueueData(vs, vt, h, f, edgeError, p, objIndex));

                    LOG(logINFO) << vs.idx() << "   " << vt.idx() << "   " << "error : " << edgeError;
                    h = topologicalMesh->next_halfedge_handle(h);
                }
            }


//            for (unsigned int i = 0; i < numTriangles; i++)
//            {
//                const Ra::Core::Face_ptr& f = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_face.at( i );
//                Ra::Core::HalfEdge_ptr h = f->HE();
//                for (j = 0; j < 3; j++)
//                {
//                    const Ra::Core::Vertex_ptr& vs = h->V();
//                    const Ra::Core::Vertex_ptr& vt = h->Next()->V();

//                    // To prevent adding twice the same edge
//                    if (vs->idx > vt->idx)
//                    {
//                        h = h->Next();
//                        continue;
//                    }

//                    if (vs->idx == 190 || vt->idx == 190)
//                    {
//                       LOG(logINFO) << "CONTACT";
//                    }


//                    // test if the edge can be collapsed or if it has contact
//                    int vertexIndex = -1;
//                    bool contact = false;
//                    Ra::Core::ProgressiveMesh<>::Primitive qk;
//                    Scalar dist;
//                    Scalar weight;
//                    Scalar sumWeight = 0;

//                    // for each vertex, we look for all contacts with other objects and add the contact quadrics to the quadric of the vertex
//                    Ra::Core::ProgressiveMesh<>::Primitive qc = Ra::Core::ProgressiveMesh<>::Primitive();
//                    for (uint k=0; k<m_kdtrees.size(); k++)
//                    {
//                        if (k != objIndex)
//                        {
//                            MeshContactElement* otherObj = static_cast<MeshContactElement*>(m_meshContactElements[k]);

//                            vertexIndex = obj->getProgressiveMeshLOD()->getProgressiveMesh()->vertexContact(vs->idx, m_kdtrees, k, m_threshold);
//                            if ( vertexIndex != -1)
//                            {
//                                contact = true;
//                                const Ra::Core::Vertex_ptr& c = otherObj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[vertexIndex];
//                                //dist = (c->P() - vs->P()).norm(); //in kdtree.hpp, the distance is a squared distance
//                                dist = (c->P() - vs->P()).squaredNorm();
//                                weight = std::pow(std::pow(dist/m_threshold,2)-1,2);
//                                //weight = 1;
//                                sumWeight += weight;
//                                qk = otherObj->getPrimitive(vertexIndex);
//                                qk *= weight;
//                                qc += qk;
//                            }

//                            vertexIndex = obj->getProgressiveMeshLOD()->getProgressiveMesh()->vertexContact(vt->idx, m_kdtrees, k, m_threshold);
//                            if ( vertexIndex != -1)
//                            {
//                                contact = true;
//                                const Ra::Core::Vertex_ptr& c = otherObj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[vertexIndex];
//                                //dist = (c->P() - vt->P()).norm();
//                                dist = (c->P() - vt->P()).squaredNorm();
//                                weight = std::pow(std::pow(dist/m_threshold,2)-1,2);
//                                //weight = 1;
//                                sumWeight += weight;
//                                qk = otherObj->getPrimitive(vertexIndex);
//                                qk *= weight;
//                                qc += qk;
//                            }
//                        }
//                    }

//                    Ra::Core::ProgressiveMesh<>::Primitive qe = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeQuadric(h->idx);
//                    Ra::Core::ProgressiveMesh<>::Primitive q = Ra::Core::ProgressiveMesh<>::Primitive(qe);
//                    if (contact)
//                    {
//                        Ra::Core::EFIterator efIt = Ra::Core::EFIterator(h);
//                        Ra::Core::FaceList facesAdj = efIt.list();
//                        int nbFacesAdj = facesAdj.size();
//                        q *= m_lambda * nbFacesAdj;
//                        qc *= 1 - m_lambda;
//                        q += qc;
//                        q *= 1 / (m_lambda * nbFacesAdj + (1 - m_lambda) * sumWeight);
//                    }

//                    edgeError = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeErrorContact(h->idx, p, q);

//                    //insert into the priority queue with the real resulting point
//                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeErrorContact(h->idx, p, qe);
//                    pQueue.insert(Ra::Core::PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, edgeError, p, objIndex));

//                    LOG(logINFO) << vs->idx << "   " << vt->idx << "   " << "error : " << edgeError;
//                    h = h->Next();
//                }
//            }
            obj->setPriorityQueue(pQueue);
            }
        }

        void MeshContactManager::updatePriorityQueue(Ra::Core::TopologicalMesh::VertexHandle vsHandle, Ra::Core::TopologicalMesh::VertexHandle vtHandle, int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);
            obj->getPriorityQueue()->removeEdges(vsHandle);
            obj->getPriorityQueue()->removeEdges(vtHandle);

            Scalar edgeError;
            Ra::Core::Vector3 p = Ra::Core::Vector3::Zero();
            Ra::Core::TopologicalMesh::VertexHandle vHandle;

            Ra::Core::TopologicalMesh* topologicalMesh = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh();

            //Listing of all the new edges formed with vs
            std::vector<Ra::Core::TopologicalMesh::HalfedgeHandle> adjHE;
            Ra::Core::TopologicalMesh::VertexOHalfedgeIter voh_it = topologicalMesh->voh_iter(vsHandle);
            for(; voh_it.is_valid(); ++voh_it) {
                adjHE.push_back(voh_it);
            }

            // test if vs has any contacts
            int vertexIndex;
            bool contactVs = false;
            Ra::Core::ProgressiveMesh<>::Primitive qk;
            Scalar dist;
            Scalar weight;
            Scalar sumWeightVs = 0;

            Ra::Core::ProgressiveMesh<>::Primitive qVs = Ra::Core::ProgressiveMesh<>::Primitive();
            for (uint k=0; k<m_kdtrees.size(); k++)
            {
                if (k != objIndex)
                {
                    MeshContactElement* otherObj = static_cast<MeshContactElement*>(m_meshContactElements[k]);

                    vertexIndex = obj->getProgressiveMeshLOD()->getProgressiveMesh()->vertexContact(vsHandle, m_kdtrees, k, m_threshold);
                    if ( vertexIndex != -1)
                    {
                        contactVs = true;

                        const Ra::Core::TopologicalMesh::VertexHandle& c = topologicalMesh->vertex_handle(vertexIndex);
                        //dist = (c->P() - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[vsIndex]->P()).norm();
                        dist = (Ra::Core::convertVec3OpenMeshToEigen(topologicalMesh->point(c) - topologicalMesh->point(vsHandle))).squaredNorm();
                        weight = std::pow(std::pow(dist/m_threshold,2)-1,2);
                        //weight = 1;
                        sumWeightVs += weight;
                        qk = otherObj->getPrimitive(vertexIndex);
                        qk *= weight;
                        qVs += qk;
                    }
                }
            }

            // test if the other vertex of the edge has any contacts
            for (uint i = 0; i < adjHE.size(); i++)
            {
                Ra::Core::TopologicalMesh::HalfedgeHandle he = adjHE[i];
                vHandle = topologicalMesh->to_vertex_handle(he);
//                Ra::Core::HalfEdge_ptr he = adjHE[i];
//                int vIndex = he->Next()->V()->idx;

                bool contact = false;
                Scalar sumWeightV = 0;
                Ra::Core::ProgressiveMesh<>::Primitive qV = Ra::Core::ProgressiveMesh<>::Primitive();

                for (uint k=0; k<m_kdtrees.size(); k++)
                {
                    if (k != objIndex)
                    {
                        MeshContactElement* otherObj = static_cast<MeshContactElement*>(m_meshContactElements[k]);
                        vertexIndex = obj->getProgressiveMeshLOD()->getProgressiveMesh()->vertexContact(vtHandle, m_kdtrees, k, m_threshold);
                        if ( vertexIndex != -1)
                        {
                            contact = true;

                            const Ra::Core::TopologicalMesh::VertexHandle& c = topologicalMesh->vertex_handle(vertexIndex);
                            //dist = (c->P() - obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_vertex[vtIndex]->P()).norm();
                            dist = (Ra::Core::convertVec3OpenMeshToEigen(topologicalMesh->point(c) - topologicalMesh->point(vtHandle))).squaredNorm();
                            weight = std::pow(std::pow(dist/m_threshold,2)-1,2);
                            //weight = 1;
                            sumWeightV += weight;
                            qk = otherObj->getPrimitive(vertexIndex);
                            qk *= weight;
                            qV += qk;
                        }
                    }
                }

                Ra::Core::ProgressiveMesh<>::Primitive qe = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeQuadric(he);
                Ra::Core::ProgressiveMesh<>::Primitive qc = Ra::Core::ProgressiveMesh<>::Primitive();
                Ra::Core::ProgressiveMesh<>::Primitive q = Ra::Core::ProgressiveMesh<>::Primitive(qe);
                Scalar sumWeight = sumWeightVs + sumWeightV;
                if (contactVs)
                {
                    qc += qVs;
                }
                if (contact)
                {
                    qc += qV;
                }
                if (contactVs || contact)
                {
                    Ra::Core::TopologicalMesh::VertexHandle vh_vs = topologicalMesh->from_vertex_handle(he);
                    Ra::Core::TopologicalMesh::VertexHandle vh_vt = topologicalMesh->to_vertex_handle(he);

                    std::vector<Ra::Core::TopologicalMesh::FaceHandle> facesAdj;
                    Ra::Core::TopologicalMesh::VertexFaceIter vf_it = topologicalMesh->vf_iter(vh_vs);
                    for(; vf_it.is_valid(); ++vf_it) {
                        facesAdj.push_back(vf_it);
                    }
                    vf_it = topologicalMesh->vf_iter(vh_vt);
                    for(; vf_it.is_valid(); ++vf_it) {
                        facesAdj.push_back(vf_it);
                    }

//                    Ra::Core::EFIterator efIt = Ra::Core::EFIterator(he);
//                    Ra::Core::FaceList facesAdj = efIt.list();

                    int nbFacesAdj = facesAdj.size();
                    q *= m_lambda * nbFacesAdj;
                    qc *= 1 - m_lambda;
                    q += qc;
                    q *= 1 / (m_lambda * nbFacesAdj + (1 - m_lambda) * sumWeight);
                }

                // compute the error while considering contacts in the quadric
                edgeError = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeErrorContact(he, p, q);

                // insert into the priority queue with the real resulting point
                obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeEdgeErrorContact(he, p, qe);

                // check that the index of the starting point of the edge is smaller than the index of its ending point
                if (vsHandle.idx() < vHandle.idx())
                {
                    obj->getPriorityQueue()->insert(Ra::Core::PriorityQueue::PriorityQueueData(vsHandle, vHandle, he, topologicalMesh->face_handle(he), edgeError, p, objIndex));
//                    obj->getPriorityQueue()->insert(Ra::Core::PriorityQueue::PriorityQueueData(vsIndex, vIndex, he->idx, he->F()->idx, edgeError, p, objIndex));
                }
                else
                {
                    obj->getPriorityQueue()->insert(Ra::Core::PriorityQueue::PriorityQueueData(vHandle, vsHandle, topologicalMesh->opposite_halfedge_handle(he), topologicalMesh->face_handle(topologicalMesh->opposite_halfedge_handle(he)), edgeError, p, objIndex));
//                    obj->getPriorityQueue()->insert(Ra::Core::PriorityQueue::PriorityQueueData(vIndex, vsIndex, he->Twin()->idx, he->Twin()->F()->idx, edgeError, p, objIndex));
                }
            }
        }

        bool MeshContactManager::edgeCollapse(int objIndex)
        {
            MeshContactElement* obj = static_cast<MeshContactElement*>(m_meshContactElements[objIndex]);
            Ra::Core::TopologicalMesh* topologicalMesh = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh();


            if (obj->isConstructM0())
            {

                //edge collapse and putting the collapse data in the ProgressiveMeshLOD
                Ra::Core::PriorityQueue::PriorityQueueData d = obj->getPriorityQueue()->top();

                Ra::Core::TopologicalMesh::HalfedgeHandle he = d.m_edge;
//                Ra::Core::HalfEdge_ptr he = obj->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()->m_halfedge[d.m_edge_id];

                 if(!topologicalMesh->is_collapse_ok(d.m_edge))
                 {
                     LOG(logINFO) << "EDGE COLLAPSE ERROR";
                     return false;
                 }

                //retrieve the quadric of vt to store it into data
                Ra::Core::TopologicalMesh::VertexHandle vt = topologicalMesh->to_vertex_handle(he);
                Ra::Core::Quadric<3> qVt = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeVertexQuadric(vt);
//                Ra::Core::Vertex_ptr vt = he->Next()->V();
//                Ra::Core::Quadric<3> qVt = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeVertexQuadric(vt->idx);
    //            Ra::Core::ProgressiveMesh<>::Primitive qVt = obj->getProgressiveMeshLOD()->getProgressiveMesh()->computeVertexQuadric(vt->idx);

                if (topologicalMesh->is_boundary(topologicalMesh->edge_handle(he)))
//                if (he->Twin() == nullptr)

                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                    obj->getProgressiveMeshLOD()->oneVertexSplitPossible();
                }
                else
                {
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                    obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseFace();
                }
                obj->getProgressiveMeshLOD()->getProgressiveMesh()->collapseVertex();
                Ra::Core::ProgressiveMeshData data = Ra::Core::TMOperations::edgeCollapse(*topologicalMesh, d.m_edge, d.m_p_result);

                //adding the quadric of vt to data
                data.setQVt(qVt);

                if (obj->getProgressiveMeshLOD()->getProgressiveMesh()->getNbFaces() > 0)
                {
                obj->getProgressiveMeshLOD()->getProgressiveMesh()->updateFacesQuadrics(d.m_vs);
                }
                //update the priority queue of the object

                updatePriorityQueue(d.m_vs, d.m_vt, objIndex);
    //            else
    //            {
    //                while (obj->getPriorityQueue()->size() > 0)
    //                    obj->getPriorityQueue()->top();
    //            }
                obj->getProgressiveMeshLOD()->addData(data);
                obj->getProgressiveMeshLOD()->oneEdgeCollapseDone();

                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
