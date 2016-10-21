#include "ProgressiveMesh.hpp"

#include <Core/RaCore.hpp>

#include <Core/Log/Log.hpp>

#include <Core/Mesh/Wrapper/Convert.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
#include <Core/Mesh/DCEL/Operations/VertexSplit.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Edge/EFIterator.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

namespace Ra
{
    namespace Core
    {

        template<class ErrorMetric>
        ProgressiveMesh<ErrorMetric>::ProgressiveMesh(TriangleMesh* mesh)
        {
            m_dcel = new Dcel();
            m_primitives.reserve(mesh->m_triangles.size());
            m_em = ErrorMetric();
            m_nb_faces = mesh->m_triangles.size();
            m_nb_vertices = mesh->m_vertices.size();
            convert(*mesh, *m_dcel);
        }

        //------------------------------------------------

        template <class ErrorMetric>
        inline Dcel* ProgressiveMesh<ErrorMetric>::getDcel()
        {
            return m_dcel;
        }

        template <class ErrorMetric>
        inline int ProgressiveMesh<ErrorMetric>::getNbFaces()
        {
            return m_nb_faces;
        }

        //------------------------------------------------

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::computeFacesQuadrics()
        {
            const uint numTriangles = m_dcel->m_face.size();
            for (uint t = 0; t < numTriangles; ++t)
            {
                Primitive q;
                m_em.generateFacePrimitive(q, m_dcel->m_face[t]);
                m_primitives.push_back(q);
            }
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::updateFacesQuadrics(Index vsIndex)
        {
            // We go all over the faces which contain vsIndex
            VFIterator vsfIt = VFIterator(m_dcel->m_vertex[vsIndex]);
            FaceList adjFaces = vsfIt.list();
            for (uint t = 0; t < adjFaces.size(); ++t)
            {
                Primitive q;
                m_em.generateFacePrimitive(q, adjFaces[t]);
                m_primitives[adjFaces[t]->idx] = q;
            }
        }

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::getWedgeAngle(Index faceIndex, Index vsIndex, Index vtIndex)
        {
            Scalar wedgeAngle;
            Face_ptr face = m_dcel->m_face[faceIndex];
            Vertex_ptr vs = m_dcel->m_vertex[vsIndex];
            Vertex_ptr vt = m_dcel->m_vertex[vtIndex];

            HalfEdge_ptr he = face->HE();
            for (int i = 0; i < 3; i++)
            {
                if (he->V() == vs || he->V() == vt)
                {
                    Vector3 v0 = he->Next()->V()->P() - he->V()->P();
                    Vector3 v1 = he->Prev()->V()->P() - he->V()->P();
                    v0.normalize();
                    v1.normalize();
                    wedgeAngle = std::acos(v0.dot(v1));
                    break;
                }
                he = he->Next();
            }
            return wedgeAngle;
        }

        template <class ErrorMetric>
        typename ErrorMetric::Primitive ProgressiveMesh<ErrorMetric>::computeEdgeQuadric(Index halfEdgeIndex)
        {
            EFIterator eIt = EFIterator(m_dcel->m_halfedge[halfEdgeIndex]);
            FaceList adjFaces = eIt.list();

            // We go all over the faces which contain vs and vt
            // We add the quadrics of all the faces
            Primitive q;
            Index fIdx;

            for (unsigned int i = 0; i < adjFaces.size(); i++)
            {
                Face_ptr f = adjFaces[i];
                fIdx = f->idx;
//                Scalar area = Ra::Core::Geometry::triangleArea
//                                ( f->HE()->V()->P(),
//                                  f->HE()->Next()->V()->P(),
//                                  f->HE()->Prev()->V()->P());
                Scalar wedgeAngle = getWedgeAngle(fIdx,
                                                m_dcel->m_halfedge[halfEdgeIndex]->V()->idx,
                                                m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->idx);
                q += m_primitives[ fIdx ] * wedgeAngle;
            }

            return q; // * (1.0/double(adjFaces.size()));
        }

        //-----------------------------------------------------

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::computeEdgeError(Index halfEdgeIndex, Vector3 &pResult)
        {
            Primitive q = computeEdgeQuadric(halfEdgeIndex);
            Vector3 vs = m_dcel->m_halfedge[halfEdgeIndex]->V()->P();
            Vector3 vt = m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();
            return m_em.computeError(q, vs, vt, pResult);
        }

        //--------------------------------------------------

        template <class ErrorMetric>
        PriorityQueue ProgressiveMesh<ErrorMetric>::constructPriorityQueue()
        {
            PriorityQueue pQueue = PriorityQueue();
            const uint numTriangles = m_dcel->m_face.size();
            //pQueue.reserve(numTriangles*3 / 2);

            // parcours des aretes
            double edgeError;
            Vector3 p = Vector3::Zero();
            int j;
#pragma omp parallel for private(j, edgeError, p)
            for (unsigned int i = 0; i < numTriangles; i++)
            {
                const Face_ptr& f = m_dcel->m_face.at( i );
                HalfEdge_ptr h = f->HE();
                for (j = 0; j < 3; j++)
                {
                    const Vertex_ptr& vs = h->V();
                    const Vertex_ptr& vt = h->Next()->V();

                    // To prevent adding twice the same edge
                    if (vs->idx > vt->idx) continue;

                    edgeError = computeEdgeError(f->HE()->idx, p);
#pragma omp critical
                    {
                        pQueue.insert(PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, edgeError, p));
                    }
                    h = h->Next();
                }
            }
            return pQueue;
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::updatePriorityQueue(PriorityQueue &pQueue, Index vsIndex, Index vtIndex)
        {
            // we delete of the priority queue all the edge containing vs_id or vt_id
            pQueue.removeEdges(vsIndex);
            pQueue.removeEdges(vtIndex);

            double edgeError;
            Vector3 p = Vector3::Zero();
            Index vIndex;

            VHEIterator vsHEIt = VHEIterator(m_dcel->m_vertex[vsIndex]);
            HalfEdgeList adjHE = vsHEIt.list();

            for (uint i = 0; i < adjHE.size(); i++)
            {
                HalfEdge_ptr he = adjHE[i];
                edgeError = computeEdgeError(he->idx, p);
                vIndex = he->Next()->V()->idx;
                pQueue.insert(PriorityQueue::PriorityQueueData(vsIndex, vIndex, he->idx, he->F()->idx, edgeError, p));
            }
            //pQueue.display();
        }

        //--------------------------------------------------

        template <class ErrorMetric>
        bool ProgressiveMesh<ErrorMetric>::isEcolPossible(Index halfEdgeIndex, Vector3 pResult)
        {
            HalfEdge_ptr he = m_dcel->m_halfedge[halfEdgeIndex];

            // Look at configuration T inside a triangle
            bool hasTIntersection = false;
            VVIterator v1vIt = VVIterator(he->V());
            VVIterator v2vIt = VVIterator(he->Next()->V());
            VertexList adjVerticesV1 = v1vIt.list();
            VertexList adjVerticesV2 = v2vIt.list();

            uint countIntersection = 0;
            for (uint i = 0; i < adjVerticesV1.size(); i++)
            {
                for (uint j = 0; j < adjVerticesV2.size(); j++)
                {
                    if (adjVerticesV1[i]->idx == adjVerticesV2[j]->idx)
                        countIntersection++;
                }
            }
            if (countIntersection > 2)
                hasTIntersection = true;

            // Look if normals of faces change after collapse
            bool isFlipped = false;
            EFIterator eIt = EFIterator(he);
            FaceList adjFaces = eIt.list();

            Index vsId = he->V()->idx;
            Index vtId = he->Next()->V()->idx;
            for (uint i = 0; i < adjFaces.size(); i++)
            {
                HalfEdge_ptr heCurr = adjFaces[i]->HE();
                Vertex_ptr v1 = nullptr;
                Vertex_ptr v2 = nullptr;
                Vertex_ptr v = nullptr;
                for (uint j = 0; j < 3; j++)
                {
                    if (heCurr->V()->idx != vsId && heCurr->V()->idx != vtId)
                    {
                        if (v1 == nullptr)
                            v1 = heCurr->V();
                        else if (v2 == nullptr)
                            v2 = heCurr->V();
                    }
                    else
                    {
                        v = heCurr->V();
                    }
                    heCurr = heCurr->Next();
                }
                if (v1 != nullptr && v2 != nullptr)
                {
                    Vector3 d1 = v1->P() - pResult;
                    Vector3 d2 = v2->P() - pResult;
                    d1.normalize();
                    d2.normalize();

                    //TEST
                    //Do we really need this ?
                    /*
                    Scalar a = fabs(d1.dot(d2));
                    Vector3 d1_before = v1->P() - v->P();
                    Vector3 d2_before = v2->P() - v->P();
                    d1_before.normalize();
                    d2_before.normalize();
                    Scalar a_before = fabs(d1_before.dot(d2_before));
                    if (a > 0.999 && a_before < 0.999)
                        isFlipped = true;
                    */

                    Vector3 fp_n = d1.cross(d2);
                    fp_n.normalize();
                    Vector3 f_n = Geometry::triangleNormal(v->P(), v1->P(), v2->P());
                    if (fp_n.dot(f_n) < 0.1)
                        isFlipped = true;

                }
            }

            return ((!hasTIntersection) && (!isFlipped));
        }

        //--------------------------------------------------

        template <class ErrorMetric>
        std::vector<ProgressiveMeshData> ProgressiveMesh<ErrorMetric>::constructM0(int targetNbFaces, int &nbNoFrVSplit)
        {
            uint nbPMData = 0;

            std::vector<ProgressiveMeshData> pmdata;
            pmdata.reserve(targetNbFaces);

            LOG(logINFO) << "Computing Faces Quadrics";
            computeFacesQuadrics();

            LOG(logINFO) << "Computing Priority Queue";
            PriorityQueue pQueue = constructPriorityQueue();
            PriorityQueue::PriorityQueueData d;

            LOG(logINFO) << "Collapsing...";
            ProgressiveMeshData pmData;
            while (m_nb_faces > targetNbFaces)
            {
                if (pQueue.empty()) break;
                d = pQueue.top();

                HalfEdge_ptr he = m_dcel->m_halfedge[d.m_edge_id];

                // TODO !
                if (!isEcolPossible(he->idx, d.m_p_result))
                {
                    LOG(logINFO) << "This edge is not collapsable for now";
                    continue;
                }

                if (he->Twin() == nullptr)
                {
                    m_nb_faces -= 1;
                    nbNoFrVSplit++;
                }
                else
                {
                    m_nb_faces -= 2;
                }
                m_nb_vertices -= 1;

                pmData = DcelOperations::edgeCollapse(*m_dcel, d.m_edge_id, d.m_p_result);
                updateFacesQuadrics(d.m_vs_id);
                updatePriorityQueue(pQueue, d.m_vs_id, d.m_vt_id);

                pmdata.push_back(pmData);

                nbPMData++;
            }
            LOG(logINFO) << "Collapsing done";

            return pmdata;
        }

        //--------------------------------------------------

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::vsplit(ProgressiveMeshData pmData)
        {
            HalfEdge_ptr he = m_dcel->m_halfedge[pmData.getHeFlId()];
            if (he->Twin() == NULL)
                m_nb_faces += 1;
            else
                m_nb_faces += 2;
            m_nb_vertices += 1;

            //LOG(logINFO) << "Vertex Split " << pmData.getVsId() << ", " << pmData.getVtId() << ", faces " << pmData.getFlId() << ", " << pmData.getFrId();

            DcelOperations::vertexSplit(*m_dcel, pmData);
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::ecol(ProgressiveMeshData pmData)
        {
            HalfEdge_ptr he = m_dcel->m_halfedge[pmData.getHeFlId()];
            if (he->Twin() == NULL)
                m_nb_faces -= 1;
            else
                m_nb_faces -= 2;
            m_nb_vertices -= 1;

            //LOG(logINFO) << "Edge Collapse " << pmData.getVsId() << ", " << pmData.getVtId() << ", faces " << pmData.getFlId() << ", " << pmData.getFrId();

            DcelOperations::edgeCollapse(*m_dcel, pmData);
        }
    }
}

