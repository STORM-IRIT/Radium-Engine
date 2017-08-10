#include "ProgressiveMesh.hpp"

#include <Core/RaCore.hpp>

#include <Core/Log/Log.hpp>

//#include <Core/Mesh/Wrapper/Convert.hpp>
#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeCollapse.hpp>
#include <Core/Mesh/TopologicalTriMesh/Operations/VertexSplit.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra
{
    namespace Core
    {


        template<class ErrorMetric>
        ProgressiveMesh<ErrorMetric>::ProgressiveMesh(TriangleMesh* mesh)
        {
            m_topologicalMesh = new TopologicalMesh();
            m_em = ErrorMetric(100.0);
            m_nb_faces = mesh->m_triangles.size();
            m_nb_vertices = mesh->m_vertices.size();
            MeshConverter::convert(*mesh, *m_topologicalMesh);
            m_bbox_size = computeBoundingBoxSize();
        }

        //------------------------------------------------

        template <class ErrorMetric>
        inline TopologicalMesh* ProgressiveMesh<ErrorMetric>::getTopologicalMesh()
        {
            return m_topologicalMesh;
        }

        template <class ErrorMetric>
        inline int ProgressiveMesh<ErrorMetric>::getNbFaces()
        {
            return m_nb_faces;
        }

        template <class ErrorMetric>
        inline ErrorMetric ProgressiveMesh<ErrorMetric>::getEM()
        {
            return m_em;
        }

        template <class ErrorMetric>
        inline std::vector<typename ErrorMetric::Primitive> ProgressiveMesh<ErrorMetric>::getFacesQuadrics()
        {
            return m_primitives;
        }

        //------------------------------------------------

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::computeBoundingBoxSize()
        {
            Scalar min_x, max_x, min_y, max_y, min_z, max_z;
            min_x = max_x = m_topologicalMesh->point(m_topologicalMesh->vertices_begin())[0];
            min_y = max_y = m_topologicalMesh->point(m_topologicalMesh->vertices_begin())[1];
            min_z = max_z = m_topologicalMesh->point(m_topologicalMesh->vertices_begin())[2];
            for (TopologicalMesh::VertexIter v_it=m_topologicalMesh->vertices_begin(); v_it!=m_topologicalMesh->vertices_end(); ++v_it)
            {

                if (m_topologicalMesh->point(v_it)[0] < min_x) min_x = m_topologicalMesh->point(v_it)[0];
                if (m_topologicalMesh->point(v_it)[0] > max_x) max_x = m_topologicalMesh->point(v_it)[0];
                if (m_topologicalMesh->point(v_it)[1] < min_y) min_y = m_topologicalMesh->point(v_it)[1];
                if (m_topologicalMesh->point(v_it)[1] > max_y) max_y = m_topologicalMesh->point(v_it)[1];
                if (m_topologicalMesh->point(v_it)[2] < min_z) min_z = m_topologicalMesh->point(v_it)[2];
                if (m_topologicalMesh->point(v_it)[2] > max_z) max_z = m_topologicalMesh->point(v_it)[2];
            }

            Vector3 size = Vector3(max_x-min_x, max_y-min_y, max_z-min_z);
            //Vector3 center = Vector3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
            return size.norm();
        }

        //------------------------------------------------



        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::computeFacesQuadrics()
        {
            const uint numTriangles = m_topologicalMesh->n_faces();
            //const uint numTriangles = m_dcel->m_face.size();

            m_primitives.clear();
            m_primitives.reserve(numTriangles);

            Primitive q;
//#pragma omp parallel for private (q)
            for (uint t = 0; t < numTriangles; ++t)
            {
                m_em.generateFacePrimitive(q,m_topologicalMesh->face_handle(t),*m_topologicalMesh);
                //m_em.generateFacePrimitive(q, m_dcel->m_face[t], *m_dcel);

//#pragma omp critical
                m_primitives.push_back(q);
            }
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::updateFacesQuadrics(TopologicalMesh::VertexHandle vsHandle)
        {
            // We go all over the faces which contain vsIndex
            TopologicalMesh::VertexFaceIter vf_it = m_topologicalMesh->vf_iter(vsHandle);
            for(; vf_it.is_valid(); ++vf_it) {
                std::cout << "Halfedge has handle " << *vf_it << std::endl;
                Primitive q;
                m_em.generateFacePrimitive(q, vf_it, *m_topologicalMesh);
                m_primitives[vf_it->idx()] = q;
            }
        }

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::getWedgeAngle(TopologicalMesh::FaceHandle faceHandle, TopologicalMesh::VertexHandle vsHandle, TopologicalMesh::VertexHandle vtHandle)
        {
            Scalar wedgeAngle;

            TopologicalMesh::FaceHalfedgeIter fh_it = m_topologicalMesh->fh_iter(faceHandle);
            for(; fh_it.is_valid(); ++fh_it) {
                if(m_topologicalMesh->to_vertex_handle(fh_it) == vsHandle || m_topologicalMesh->to_vertex_handle(fh_it) == vtHandle)
                {
                    Vector3 v0 = m_topologicalMesh->point(m_topologicalMesh->from_vertex_handle(m_topologicalMesh->next_halfedge_handle(fh_it))) - m_topologicalMesh->point(m_topologicalMesh->from_vertex_handle(fh_it));
                    Vector3 v1 = m_topologicalMesh->point(m_topologicalMesh->from_vertex_handle(m_topologicalMesh->prev_halfedge_handle(fh_it))) - m_topologicalMesh->point(m_topologicalMesh->from_vertex_handle(fh_it));
                    v0.normalize();
                    v1.normalize();
                    wedgeAngle = std::acos(v0.dot(v1));
                    break;
                }
            }

            CORE_ASSERT(wedgeAngle < 360, "WEDGE ANGLE WAY TOO HIGH");

            return wedgeAngle;
        }

        template <class ErrorMetric>
        typename ErrorMetric::Primitive ProgressiveMesh<ErrorMetric>::computeEdgeQuadric(TopologicalMesh::HalfedgeHandle halfEdgeHandle)
        {

            TopologicalMesh::VertexHandle vh_vs = m_topologicalMesh->from_vertex_handle(halfEdgeHandle);
            TopologicalMesh::VertexHandle vh_vt = m_topologicalMesh->to_vertex_handle(halfEdgeHandle);

            std::vector<TopologicalMesh::FaceHandle> adjFaces;
            TopologicalMesh::VertexFaceIter vf_it = m_topologicalMesh->vf_iter(vh_vs);
            for(; vf_it.is_valid(); ++vf_it) {
                adjFaces.push_back(vf_it);
            }
            vf_it = m_topologicalMesh->vf_iter(vh_vt);
            for(; vf_it.is_valid(); ++vf_it) {
                adjFaces.push_back(vf_it);
            }

            // We go all over the faces which contain vs and vt
            // We add the quadrics of all the faces
            Primitive q, qToAdd;
            Index fIdx;

            Scalar weight = 1.0/adjFaces.size();
            q = m_primitives[adjFaces[0].idx()];
            q *= weight;

            for (unsigned int i = 1; i < adjFaces.size(); i++)
            {
                TopologicalMesh::FaceHandle f = adjFaces[i];
                fIdx = f.idx();
//                Scalar area = Ra::Core::Geometry::triangleArea
//                                ( f->HE()->V()->P(),
//                                  f->HE()->Next()->V()->P(),
//                                  f->HE()->Prev()->V()->P());
//                Scalar wedgeAngle = getWedgeAngle(fIdx,
//                                                m_dcel->m_halfedge[halfEdgeIndex]->V()->idx,
//                                                m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->idx);
                qToAdd = m_primitives[fIdx];
                qToAdd *= weight;

                q += qToAdd;
            }

            return q;
        }

        template <class ErrorMetric>
        typename ErrorMetric::Primitive ProgressiveMesh<ErrorMetric>::computeVertexQuadric(TopologicalMesh::VertexHandle vertexHandle)
        {
            // We go all over the faces which contain vertexIndex

            std::vector<TopologicalMesh::FaceHandle> adjFaces;
            TopologicalMesh::VertexFaceIter vf_iter = m_topologicalMesh->vf_iter(vertexHandle);
            for(; vf_iter.is_valid(); ++vf_iter) {
                adjFaces.push_back(vf_iter);
            }


            Primitive q = m_primitives[adjFaces[0].idx()];
            for (uint i = 1; i < adjFaces.size(); i++)
            {
                q += m_primitives[adjFaces[i].idx()];
            }

            Scalar weight = 1.0/adjFaces.size();
            q *= weight;

            return q;
        }


        //-----------------------------------------------------

        template <class ErrorMetric>
        bool ProgressiveMesh<ErrorMetric>::isPlanarEdge(TopologicalMesh::HalfedgeHandle halfEdgeHandle)
        {
            return m_em.isPlanarEdge(halfEdgeHandle, m_topologicalMesh);
        }

        template <class ErrorMetric>
        bool ProgressiveMesh<ErrorMetric>::isPlanarEdge2(TopologicalMesh::HalfedgeHandle halfEdgeHandle, TopologicalMesh::VertexHandle &vsHandle, TopologicalMesh::VertexHandle &vtHandle)
        {
            return m_em.isPlanarEdge2(halfEdgeHandle, m_topologicalMesh, vsHandle, vtHandle);
        }


        //-----------------------------------------------------

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::computeEdgeError(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 &pResult)
        {
            Primitive q = computeEdgeQuadric(halfEdgeHandle);
//            Vector3 vs = m_dcel->m_halfedge[halfEdgeIndex]->V()->P();
//            Vector3 vt = m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();
//            return m_em.computeError(q, vs, vt, pResult);
            return m_em.computeError(q, halfEdgeHandle, pResult, m_topologicalMesh);
        }

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::computeEdgeErrorContact(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 &pResult, Primitive q)
        {

//            Vector3 vs = m_dcel->m_halfedge[halfEdgeIndex]->V()->P();
//            Vector3 vt = m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();
//            return m_em.computeError(q, vs, vt, pResult);
            return m_em.computeError(q, halfEdgeHandle, pResult, m_topologicalMesh);
        }     

        //--------------------------------------------------

        template <class ErrorMetric>
        int ProgressiveMesh<ErrorMetric>::vertexContact(TopologicalMesh::VertexHandle vertexHandle, std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idxOtherObject, Scalar threshold)
        {

            // Look if the vertex is too close to another object
            const Super4PCS::KdTree<Scalar>::VectorType& p = reinterpret_cast<const Super4PCS::KdTree<Scalar>::VectorType&>(m_topologicalMesh->point(vertexHandle));
            int contact = kdtrees[idxOtherObject]->doQueryRestrictedClosestIndex(p, threshold);
            return contact;
        }

//        template <class ErrorMetric>
//        bool ProgressiveMesh<ErrorMetric>::hasContact(Index halfEdgeIndex, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx)
//        {

//            HalfEdge_ptr he = m_dcel->m_halfedge[halfEdgeIndex];

//            // Look if either point of the halfedge is too close to another object
//            bool contact = false;
//            Index v1 = he->V()->idx;
//            Index v2 = he->Next()->V()->idx;

//            for (uint i=0; i<kdtrees.size() && !contact; i++) //optimization possible : find the closest kdtrees
//            {
//                if (i!=idx)
//                {
//                    if (vertexContact(v1, kdtrees, i) != -1)
//                        contact = true;
//                    else
//                    {
//                        if (vertexContact(v2, kdtrees, i) != -1)
//                            contact = true;
//                    }
//                }

//                else
//                {
//                    LOG(logINFO) << "Same object";
//                }
//            }

//            if (contact)
//            {
//                LOG(logINFO) << "The edge " << v1 << ", " << v2 << " is not collapsable for now : contact found";
//            }
//            return contact;

//        }

//        template <class ErrorMetric>
//        bool ProgressiveMesh<ErrorMetric>::hasContact(Index halfEdgeIndex, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx)
//        {

//            HalfEdge_ptr he = m_dcel->m_halfedge[halfEdgeIndex];

//            // Look if either point of the halfedge is too close to another object
//            bool contact = false;
//            Vertex_ptr v1 = he->V();
//            Vertex_ptr v2 = he->Next()->V();
//            bool v1Contact = false;
//            bool v2Contact = false;
//            for (uint i=0; i<kdtrees.size() && !contact; i++) //optimization possible : find the closest kdtrees
//            {
//                if (i!=idx)
//                {
//                    LOG(logINFO) << "idx " << idx;
//                    LOG(logINFO) << "kdtree size " << kdtrees[i]->_getPoints().size();
//                    //const Super4PCS::KdTree<float>::VectorType& p1 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v1);
//                    const Super4PCS::KdTree<float>::VectorType& p1 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v1->P());
//                    LOG(logINFO) << "v1 : " << p1.transpose();
//                    //int index1 = kdtrees[i]->doQueryRestrictedClosestIndex(p1, 0.8);
//                    if (kdtrees[i]->doQueryRestrictedClosestIndex(p1, 0.00001) != -1)
//                        contact = true;

//                    else
//                    {
//                        //const Super4PCS::KdTree<float>::VectorType& p2 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v2);
//                        const Super4PCS::KdTree<float>::VectorType& p2 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v2->P());
//                        LOG(logINFO) << "v2 : " << p2.transpose();
//                        //int index2 = kdtrees[i]->doQueryRestrictedClosestIndex(p2, 0.8);
//                        if (kdtrees[i]->doQueryRestrictedClosestIndex(p2, 0.00001) != -1)
//                            contact = true;
//                    }
//                }

//                else
//                {
//                    LOG(logINFO) << "Same object";
//                }
//            }

//            if (contact)
//            {
//                LOG(logINFO) << "The edge " << v1->idx << ", " << v2->idx << " is not collapsable for now : contact found";
//            }
//            return contact;

//        }



        //--------------------------------------------------




//        template <class ErrorMetric>
//        PriorityQueue ProgressiveMesh<ErrorMetric>::constructPriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, int objIndex)
//        {
//            PriorityQueue pQueue = PriorityQueue();
//            const uint numTriangles = m_dcel->m_face.size();
//            //pQueue.reserve(numTriangles*3 / 2);

//            // parcours des aretes
//            double edgeError;
//            Vector3 p = Vector3::Zero();
//            int j;
////#pragma omp parallel for private(j, edgeError, p)
//            for (unsigned int i = 0; i < numTriangles; i++)
//            {
//                const Face_ptr& f = m_dcel->m_face.at( i );
//                HalfEdge_ptr h = f->HE();
//                for (j = 0; j < 3; j++)
//                {
//                    const Vertex_ptr& vs = h->V();
//                    const Vertex_ptr& vt = h->Next()->V();

//                    // To prevent adding twice the same edge
//                    if (vs->idx > vt->idx) continue;



//                    // test if the edge can be collapsed or if it has contact
//                    //bool isPossible = isEcolPossible(f->HE()->idx, p);
//                    bool contact = false;

//                    //if (isPossible)
//                        contact = hasContact(h->idx, kdtrees, objIndex);


//                    if (/*isPossible && */!contact)
////#pragma omp critical
//                    {
//                        edgeError = computeEdgeError(f->HE()->idx, p);
//                        pQueue.insert(PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, edgeError, p, objIndex));
//                    }

//                    //else if (isPossible && isContact)
//                    else
////#pragma omp critical
//                    {
//                        pQueue.insert(PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, 1000, p, objIndex));
//                    }

//                    LOG(logINFO) << "Halfedge " << h->idx;
//                    h = h->Next();
//                    LOG(logINFO) << "Next halfedge " << h->idx;
//                }
//            }

//            //pQueue.display();
//            return pQueue;
//        }

//        template <class ErrorMetric>
//        void ProgressiveMesh<ErrorMetric>::updatePriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, PriorityQueue &pQueue, Index vsIndex, Index vtIndex, int objIndex)
//        {
//            // we delete of the priority queue all the edge containing vs_id or vt_id
//            pQueue.removeEdges(vsIndex);
//            pQueue.removeEdges(vtIndex);

//            double edgeError;
//            Vector3 p = Vector3::Zero();
//            Index vIndex;

//            VHEIterator vsHEIt = VHEIterator(m_dcel->m_vertex[vsIndex]);
//            HalfEdgeList adjHE = vsHEIt.list();

//            for (uint i = 0; i < adjHE.size(); i++)
//            {
//                HalfEdge_ptr he = adjHE[i];

//                vIndex = he->Next()->V()->idx;
//                LOG(logINFO) << "Halfedge " << he->idx;
//                LOG(logINFO) << "Next halfedge " << vIndex;

//                // test if the edge can be collapsed or if it has contact
//                //bool isPossible = isEcolPossible(he->idx, p);
//                bool contact = false;

//                //if (isPossible)
//                    contact = hasContact(he->idx, kdtrees, objIndex);


//                if (/*isPossible && */!contact)
//                {
//                    edgeError = computeEdgeError(he->idx, p);
//                    pQueue.insert(PriorityQueue::PriorityQueueData(vsIndex, vIndex, he->idx, he->F()->idx, edgeError, p, objIndex));
//                }

//                //else if (isPossible && isContact)
//                else
//                {
//                    pQueue.insert(PriorityQueue::PriorityQueueData(vsIndex, vIndex, he->idx, he->F()->idx, 1000, p, objIndex));
//                }


//                //pQueue.insert(PriorityQueue::PriorityQueueData(vsIndex, vIndex, he->idx, he->F()->idx, edgeError, p, objIndex));
//            }
//            //pQueue.display();
//        }

        //--------------------------------------------------

        //Quadric-Based Polygonal Surface Simplification, PhD thesis by Michael Garland (1999), p.56-57 : Consistency Checks
        template <class ErrorMetric>
        bool ProgressiveMesh<ErrorMetric>::isEcolConsistent(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult)
        {

            TopologicalMesh::HalfedgeHandle heh = halfEdgeHandle;
            TopologicalMesh::FaceHandle f1 = m_topologicalMesh->face_handle(heh);
            TopologicalMesh::FaceHandle f2 = m_topologicalMesh->face_handle(m_topologicalMesh->opposite_halfedge_handle(heh));

            TopologicalMesh::VertexHandle v1 = m_topologicalMesh->from_vertex_handle(heh);
            TopologicalMesh::VertexHandle v2 = m_topologicalMesh->to_vertex_handle(heh);
            std::vector<TopologicalMesh::FaceHandle> adjFacesV1;
            std::vector<TopologicalMesh::FaceHandle> adjFacesV2;

            TopologicalMesh::VertexFaceIter vf_iter1 =  m_topologicalMesh->vf_iter(v1);
            for(; vf_iter1.is_valid(); ++vf_iter1) {
                adjFacesV1.push_back(vf_iter1);
            }
            TopologicalMesh::VertexFaceIter vf_iter2 =  m_topologicalMesh->vf_iter(v2);
            for(; vf_iter2.is_valid(); ++vf_iter2) {
                adjFacesV2.push_back(vf_iter2);
            }

            bool consistent = true;

            for (uint i = 0; i < adjFacesV1.size() && consistent; i++)
            {
               TopologicalMesh::FaceHandle f = adjFacesV1[i];
               if ((f != f1) && (f != f2))
               {
                heh = m_topologicalMesh->halfedge_handle(f);
                TopologicalMesh::VertexHandle vh = m_topologicalMesh->from_vertex_handle(heh);
                while (vh != v1)
                {
                    heh = m_topologicalMesh->next_halfedge_handle(heh);
                    vh = m_topologicalMesh->from_vertex_handle(heh);
                }
                heh = m_topologicalMesh->next_halfedge_handle(heh);
                TopologicalMesh::VertexHandle vs = m_topologicalMesh->from_vertex_handle(heh);
                TopologicalMesh::VertexHandle vt = m_topologicalMesh->to_vertex_handle(heh);

                TopologicalMesh::Point topological_vsvt = m_topologicalMesh->point(vs)- m_topologicalMesh->point(vt);
                Vector3 vsvt = convertVec3OpenMeshToEigen(topological_vsvt);
                Vector3 nf = Geometry::triangleNormal(convertVec3OpenMeshToEigen(m_topologicalMesh->point(v1)), convertVec3OpenMeshToEigen(m_topologicalMesh->point(vs)), convertVec3OpenMeshToEigen(m_topologicalMesh->point(vt)));
                Vector3 n = vsvt.cross(nf);

                consistent = ((n.dot(convertVec3OpenMeshToEigen(m_topologicalMesh->point(v1))) >= 0) == (n.dot(pResult) >= 0));
                if (! consistent)
                {
                    LOG(logINFO) << "Edge " << v1.idx() << " " << v2.idx() << " is not collapsable due to inconsistency";
                }
               }
            }

            for (uint i = 0; i < adjFacesV2.size() && consistent; i++)
            {
                TopologicalMesh::FaceHandle f = adjFacesV2[i];
                if ((f != f1) && (f != f2))
                {
                 heh = m_topologicalMesh->halfedge_handle(f);
                 TopologicalMesh::VertexHandle vh = m_topologicalMesh->from_vertex_handle(heh);
                 while (vh != v2)
                 {
                     heh = m_topologicalMesh->next_halfedge_handle(heh);
                     vh = m_topologicalMesh->from_vertex_handle(heh);
                 }
                 heh = m_topologicalMesh->next_halfedge_handle(heh);
                 TopologicalMesh::VertexHandle vs = m_topologicalMesh->from_vertex_handle(heh);
                 TopologicalMesh::VertexHandle vt = m_topologicalMesh->to_vertex_handle(heh);

                 TopologicalMesh::Point topological_vsvt = m_topologicalMesh->point(vs)- m_topologicalMesh->point(vt);
                 Vector3 vsvt = convertVec3OpenMeshToEigen(topological_vsvt);
                 Vector3 nf = Geometry::triangleNormal(convertVec3OpenMeshToEigen(m_topologicalMesh->point(v2)), convertVec3OpenMeshToEigen(m_topologicalMesh->point(vs)), convertVec3OpenMeshToEigen(m_topologicalMesh->point(vt)));
                 Vector3 n = vsvt.cross(nf);

                 consistent = ((n.dot(convertVec3OpenMeshToEigen(m_topologicalMesh->point(v2))) >= 0) == (n.dot(pResult) >= 0));
                 if (! consistent)
                 {
                     LOG(logINFO) << "Edge " << v1.idx() << " " << v2.idx() << " is not collapsable due to inconsistency";
                 }
                }
            }

           return consistent;
        }


        template <class ErrorMetric>
        bool ProgressiveMesh<ErrorMetric>::isEcolPossible(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult/*, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx*/)
        {

            TopologicalMesh::HalfedgeHandle heh = halfEdgeHandle;
            //HalfEdge_ptr he = m_dcel->m_halfedge[halfEdgeIndex];

//            // Look if either point of the halfedge is too close to another object
//            bool hasContact = false; //idx of the simplified mesh?
//            Vertex_ptr v1 = he->V();
//            Vertex_ptr v2 = he->Next()->V();
//            bool v1Contact = false;
//            bool v2Contact = false;
//            for (uint i=0; i<kdtrees.size() && !v1Contact && !v2Contact; i++) //while loop is better, optimization possible : find the closest kdtrees
//            {
//                if (i!=idx)
//                {
//                    LOG(logINFO) << "idx " << idx;
//                    LOG(logINFO) << "kdtree size " << kdtrees[i]->_getPoints().size();
//                    //const Super4PCS::KdTree<float>::VectorType& p1 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v1);
//                    const Super4PCS::KdTree<float>::VectorType& p1 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v1->P());
//                    LOG(logINFO) << "v1 : " << p1.transpose();
//                    //int index1 = kdtrees[i]->doQueryRestrictedClosestIndex(p1, 0.8);
//                    if (kdtrees[i]->doQueryRestrictedClosestIndex(p1, 0.00001) != -1)
//                        v1Contact = true;
//                    else
//                    {
//                        //const Super4PCS::KdTree<float>::VectorType& p2 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v2);
//                        const Super4PCS::KdTree<float>::VectorType& p2 = reinterpret_cast<const Super4PCS::KdTree<float>::VectorType&>(v2->P());
//                        LOG(logINFO) << "v2 : " << p2.transpose();
//                        //int index2 = kdtrees[i]->doQueryRestrictedClosestIndex(p2, 0.8);
//                        if (kdtrees[i]->doQueryRestrictedClosestIndex(p2, 0.00001) != -1)
//                            v2Contact = true;
//                    }
//                }
//                else
//                    LOG(logINFO) << "Same object";
//            }

//            if (v1Contact || v2Contact)
//            {
//                hasContact = true;
//                LOG(logINFO) << "The edge " << v1->idx << ", " << v2->idx << " is not collapsable for now : contact found";
//                return false;
//            }

            // Look at configuration T inside a triangle

            bool hasTIntersection = false;
            std::vector<TopologicalMesh::VertexHandle> adjVerticesV1;
            std::vector<TopologicalMesh::VertexHandle> adjVerticesV2;

            TopologicalMesh::VertexVertexIter vv_iter1 =  m_topologicalMesh->vv_iter(m_topologicalMesh->from_vertex_handle(halfEdgeHandle));
            for(; vv_iter1.is_valid(); ++vv_iter1) {
                adjVerticesV1.push_back(vv_iter1);
            }
            TopologicalMesh::VertexVertexIter vv_iter2 =  m_topologicalMesh->vv_iter(m_topologicalMesh->to_vertex_handle(halfEdgeHandle));
            for(; vv_iter2.is_valid(); ++vv_iter2) {
                adjVerticesV2.push_back(vv_iter2);
            }

            uint countIntersection = 0;
            for (uint i = 0; i < adjVerticesV1.size(); i++)
            {
                for (uint j = 0; j < adjVerticesV2.size(); j++)
                {
                    if (adjVerticesV1[i].idx() == adjVerticesV2[j].idx())
                        countIntersection++;
                }
            }
            if (countIntersection > 2)
            {
                LOG(logINFO) << "The edge " << m_topologicalMesh->from_vertex_handle(halfEdgeHandle).idx() << ", " << m_topologicalMesh->to_vertex_handle(halfEdgeHandle).idx() << " in face " << m_topologicalMesh->face_handle(halfEdgeHandle).idx() << " is not collapsable for now : T-Intersection";
                hasTIntersection = true;
                return false;
            }

            // Look if normals of faces change after collapse

            bool isFlipped = false;

            TopologicalMesh::VertexHandle vh_vs = m_topologicalMesh->from_vertex_handle(halfEdgeHandle);
            TopologicalMesh::VertexHandle vh_vt = m_topologicalMesh->to_vertex_handle(halfEdgeHandle);

            std::vector<TopologicalMesh::FaceHandle> adjFaces;
            TopologicalMesh::VertexFaceIter vf_it = m_topologicalMesh->vf_iter(vh_vs);
            for(; vf_it.is_valid(); ++vf_it) {
                adjFaces.push_back(vf_it);
            }
            vf_it = m_topologicalMesh->vf_iter(vh_vt);
            for(; vf_it.is_valid(); ++vf_it) {
                adjFaces.push_back(vf_it);
            }

            for (uint i = 0; i < adjFaces.size(); i++)
            {
                TopologicalMesh::HalfedgeHandle hehCurr = m_topologicalMesh->halfedge_handle(adjFaces[i]);
                TopologicalMesh::VertexHandle v1;
                TopologicalMesh::VertexHandle v2;
                TopologicalMesh::VertexHandle v;
                for (uint j = 0; j < 3; j++)
                {
                    if (m_topologicalMesh->from_vertex_handle(hehCurr).idx() != vh_vs.idx() && m_topologicalMesh->from_vertex_handle(hehCurr).idx() != vh_vt.idx())
                    {
                        if (!v1.is_valid())
                            v1 = m_topologicalMesh->from_vertex_handle(hehCurr);
                        else if (!v2.is_valid())
                            v2 = m_topologicalMesh->from_vertex_handle(hehCurr);
                    }
                    else
                    {
                        v = m_topologicalMesh->from_vertex_handle(hehCurr);
                    }
                    hehCurr = m_topologicalMesh->next_halfedge_handle(hehCurr);
                }
                if (v1.is_valid() && v2.is_valid())
                {
                    Vector3 d1 = convertVec3OpenMeshToEigen(m_topologicalMesh->point(v1)) - pResult;
                    Vector3 d2 = convertVec3OpenMeshToEigen(m_topologicalMesh->point(v2)) - pResult;
                    d1.normalize();
                    d2.normalize();

                    //TEST
                    //Do we really need this ?

                    //Scalar a = fabs(d1.dot(d2));
                    //Vector3 d1_before = v1->P() - v->P();
                    //Vector3 d2_before = v2->P() - v->P();
                    //d1_before.normalize();
                    //d2_before.normalize();
                    //Scalar a_before = fabs(d1_before.dot(d2_before));
                    //if (a > 0.999 && a_before < 0.999)
                    //    isFlipped = true;


                    Vector3 fp_n = d1.cross(d2);
                    fp_n.normalize();
                    Vector3 f_n = Geometry::triangleNormal(convertVec3OpenMeshToEigen(m_topologicalMesh->point(v)), convertVec3OpenMeshToEigen(m_topologicalMesh->point(v1)), convertVec3OpenMeshToEigen(m_topologicalMesh->point(v2)));
                    Scalar fpnDotFn = fp_n.dot(f_n);
                    if (fpnDotFn < -0.5)
                    {
                        isFlipped = true;
                        LOG(logINFO) << "The edge " << m_topologicalMesh->from_vertex_handle(halfEdgeHandle).idx() << ", " << m_topologicalMesh->to_vertex_handle(halfEdgeHandle).idx() << " in face " << m_topologicalMesh->face_handle(halfEdgeHandle).idx() << " is not collapsable for now : Flipped face";
                        return false;
                        break;
                    }

                }
            }

            //return !hasTIntersection;

            LOG(logINFO) << "edge collapse" << ((!hasTIntersection) && (!isFlipped) /*&& (!hasContact)*/);
            return ((!hasTIntersection) && (!isFlipped) /*&& (!hasContact)*/);
        }



        //--------------------------------------------------

//        template <class ErrorMetric>
//        std::vector<ProgressiveMeshData> ProgressiveMesh<ErrorMetric>::constructM0(int targetNbFaces, int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx)
//        {
//            uint nbPMData = 0;

//            std::vector<ProgressiveMeshData> pmdata;
//            pmdata.reserve(targetNbFaces);

//            LOG(logINFO) << "Computing Faces Quadrics";
//            computeFacesQuadrics();

//            LOG(logINFO) << "Computing Priority Queue";
//            PriorityQueue pQueue = constructPriorityQueue(idx);
//            PriorityQueue::PriorityQueueData d;

//            LOG(logINFO) << "Collapsing...";
//            ProgressiveMeshData data;
//            while (m_nb_faces > targetNbFaces)
//            {
//                if (pQueue.empty()) break;
//                d = pQueue.top();

//                HalfEdge_ptr he = m_dcel->m_halfedge[d.m_edge_id];

//                // TODO !
//                if (!isEcolPossible(he->idx, d.m_p_result, kdtrees, idx))
//                {
//                    LOG(logINFO) << "Collapse not possible";
//                    continue;
//                }

//                if (he->Twin() == nullptr)
//                {
//                    m_nb_faces -= 1;
//                    nbNoFrVSplit++;
//                }
//                else
//                {
//                    m_nb_faces -= 2;
//                }
//                m_nb_vertices -= 1;

//                data = DcelOperations::edgeCollapse(*m_dcel, d.m_edge_id, d.m_p_result);
//                updateFacesQuadrics(d.m_vs_id);
//                updatePriorityQueue(pQueue, d.m_vs_id, d.m_vt_id, idx);

//                pmdata.push_back(data);

//                nbPMData++;
//            }
//            LOG(logINFO) << "Collapsing done";

//            return pmdata;
//        }

        template <class ErrorMetric>
        bool ProgressiveMesh<ErrorMetric>::isConstructM0(std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idx, PriorityQueue &pQueue)
        {
            PriorityQueue::PriorityQueueData d = pQueue.firstData();
            TopologicalMesh::HalfedgeHandle heh = d.m_edge;
            if (!isEcolPossible(heh, d.m_p_result/*, kdtrees, idx*/))
            {
                LOG(logINFO) << "Collapse not possible";
                pQueue.top();
                return false;
            }
            else
                return true;
        }

        // constructM0 is called only if isConstructM0 true
//        template <class ErrorMetric>
//        ProgressiveMeshData ProgressiveMesh<ErrorMetric>::constructM0(int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue)
//        {


//            //bool ec = false;

//            PriorityQueue::PriorityQueueData d;


//            ProgressiveMeshData data;

//                d = pQueue.top();

//                HalfEdge_ptr he = m_dcel->m_halfedge[d.m_edge_id];

////                // TODO !
////                if (!isEcolPossible(he->idx, d.m_p_result/*, kdtrees, idx*/))
////                {
////                    LOG(logINFO) << "Collapse not possible";

////                }

////                else
////                {

//                                if (he->Twin() == nullptr)
//                                {
//                                    m_nb_faces -= 1;
//                                    nbNoFrVSplit++;
//                                }
//                                else
//                                {
//                                    m_nb_faces -= 2;
//                                }
//                                m_nb_vertices -= 1;

//                    LOG(logINFO) << "Edge " << d.m_vs_id << " " << d.m_vt_id;
//                    data = DcelOperations::edgeCollapse(*m_dcel, d.m_edge_id, d.m_p_result);
//                    //pmData.push_back(data);
//                    updateFacesQuadrics(d.m_vs_id);
//                    updatePriorityQueue(kdtrees, pQueue, d.m_vs_id, d.m_vt_id, idx);
//                    //ec = true;
//                    LOG(logINFO) << "Collapsing done";
//                    LOG(logINFO) << "pResult : " << d.m_p_result(0,0) << " " << d.m_p_result(1,0) << " " << d.m_p_result(2,0);

////                    pmlod.m_pmdata.push_back(data);
////                    (pmlod.m_curr_vsplit)++;

////                }

//            //return ec;

//                return data;
//        }


        //--------------------------------------------------

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::vsplit(ProgressiveMeshData pmData)
        {
            TopologicalMesh::HalfedgeHandle heh = pmData.getHeFl();
            if(m_topologicalMesh->is_boundary(m_topologicalMesh->opposite_halfedge_handle(heh)) || m_topologicalMesh->is_boundary(heh))
                m_nb_faces += 1;
            else
                m_nb_faces += 2;
            m_nb_vertices += 1;

            //LOG(logINFO) << "Vertex Split " << pmData.getVsId() << ", " << pmData.getVtId() << ", faces " << pmData.getFlId() << ", " << pmData.getFrId();

            TMOperations::vertexSplit(*m_topologicalMesh, pmData);
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::ecol(ProgressiveMeshData pmData)
        {
            TopologicalMesh::HalfedgeHandle heh = pmData.getHeFl();

            if(m_topologicalMesh->is_boundary(m_topologicalMesh->edge_handle(heh)))
                m_nb_faces -= 1;
            else
                m_nb_faces -= 2;
            m_nb_vertices -= 1;

            //LOG(logINFO) << "Edge Collapse " << pmData.getVsId() << ", " << pmData.getVtId() << ", faces " << pmData.getFlId() << ", " << pmData.getFrId();

            TMOperations::edgeCollapse(*m_topologicalMesh, pmData);
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::collapseFace()
        {
            m_nb_faces--;
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::collapseVertex()
        {
            m_nb_vertices--;
        }

    }
}

