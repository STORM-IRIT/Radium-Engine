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

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define triang(x) (((x) * ((x) + 1)) / 2)
#define ind(row, col, n) ((row) * ((n) - 2) - triang((row) - 1) + (col) - 1)

namespace Ra
{
    namespace Core
    {

        template <class Primitive>
        ProgressiveMesh<Primitive>::ProgressiveMesh(TriangleMesh* mesh)
        {
            m_dcel = new Dcel();
            m_quadrics = new Primitive[mesh->m_triangles.size()]();
            m_nb_faces = mesh->m_triangles.size();
            m_nb_vertices = mesh->m_vertices.size();
            convert(*mesh, *m_dcel);
        }

        //------------------------------------------------

        template <class Primitive>
        inline Dcel* ProgressiveMesh<Primitive>::getDcel()
        {
            return m_dcel;
        }

        template <class Primitive>
        inline int ProgressiveMesh<Primitive>::getNbFaces()
        {
            return m_nb_faces;
        }

        template <class Primitive>
        inline int ProgressiveMesh<Primitive>::getNbVertices()
        {
            return m_nb_vertices;
        }

        //------------------------------------------------

        template <class Primitive>
        void ProgressiveMesh<Primitive>::computeFacesQuadrics()
        {
            const uint numTriangles = m_dcel->m_face.size();

            Vector3 n;
#pragma omp parallel for private(n)
            for (uint t = 0; t < numTriangles; ++t)
            {
                Face_ptr f = m_dcel->m_face[t];
                Vertex_ptr v0 = f->HE()->V();
                Vertex_ptr v1 = f->HE()->Next()->V();
                Vertex_ptr v2 = f->HE()->Next()->Next()->V();

                n = Geometry::triangleNormal(v0->P(), v1->P(), v2->P());
                m_quadrics[t] = Primitive(n, -n.dot(v0->P()));
            }
        }

        template <class Primitive>
        void ProgressiveMesh<Primitive>::updateFacesQuadrics(Index vsIndex)
        {
            // We go all over the faces which contain vsIndex
            VFIterator vsfIt = VFIterator(m_dcel->m_vertex[vsIndex]);
            FaceList adjFaces = vsfIt.list();

            Vector3 n;
            for (uint t = 0; t < adjFaces.size(); ++t)
            {
                Face_ptr f = adjFaces[t];
                Vertex_ptr v0 = f->HE()->V();
                Vertex_ptr v1 = f->HE()->Next()->V();
                Vertex_ptr v2 = f->HE()->Next()->Next()->V();

                n = Geometry::triangleNormal(v0->P(), v1->P(), v2->P());

                m_quadrics[adjFaces[t]->idx] = Primitive(n,-n.dot(v0->P()));
            }

        }


        template <class Primitive>
        Scalar ProgressiveMesh<Primitive>::getWedgeAngle(Index faceIndex, Index vsIndex, Index vtIndex)
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

        template <class Primitive>
        Primitive ProgressiveMesh<Primitive>::computeEdgeQuadric(Index halfEdgeIndex)
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
                q += m_quadrics[ fIdx ] * wedgeAngle;
            }

            return q; // * (1.0/double(adjFaces.size()));
        }

        //-----------------------------------------------------

        template <class Primitive>
        Scalar ProgressiveMesh<Primitive>::computeEdgeError(Index halfEdgeIndex, Vector3 &p_result)
        {
            Primitive q = computeEdgeQuadric(halfEdgeIndex);
            Scalar error;

            // on cherche v_result
            // A v_result = -b		avec A = nn^T
            //							 b = dn
            Matrix3 A_inverse = q.getA().inverse();

            Scalar det = q.getA().determinant();
            if (det > 0.0001)
            {
                p_result = -A_inverse * q.getB();
                error = computeGeometricError(p_result, q);
            }
            else //matrix non inversible
            {
                Vector3 p1 = m_dcel->m_halfedge[halfEdgeIndex]->V()->P();
                Vector3 p2 = m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();
                Vector3 p12 = (p1 + p2) / 2.0;

                Scalar p1_error = computeGeometricError(p1, q);
                Scalar p2_error = computeGeometricError(p2, q);
                Scalar p12_error = computeGeometricError(p12, q);

                error = p1_error;
                Vector3 p = p1;
                if (p2_error < error && p12_error > p2_error)
                {
                    p = p2;
                    p_result = p;
                    error = p2_error;
                }
                else if (p12_error < error && p2_error > p12_error)
                {
                    p = p12;
                    p_result = p;
                    error = p12_error;
                }
                else
                {
                    p_result = p;
                }
            }
            return error;
        }


        template <class Primitive>
        Scalar ProgressiveMesh<Primitive>::computeGeometricError(const Vector3& p, Primitive q)
        {
            // Computing geometric error
            // v^T A v + 2 * b^T v + c
            Eigen::Matrix<Scalar, 1, 3> row_p = p.transpose();
            Eigen::Matrix<Scalar, 1, 3> row_b = q.getB().transpose();
            Scalar error_a = row_p * q.getA() * p;
            Scalar error_b = 2.0 * row_b * p;
            Scalar error_c = q.getC();
            return (error_a + error_b + error_c);
        }

        //--------------------------------------------------

        /*
        template <class Primitive>
        PriorityQueue ProgressiveMesh<Primitive>::constructPriorityQueue()
        {
            PriorityQueue pQueue = PriorityQueue();

            const uint numTriangles = m_dcel->m_face.size();
            const uint numVertices = m_dcel->m_vertex.size();

            //pQueue.reserve(numTriangles*3 / 2);

            // matrice triangulaire inferieure sans diagonale dans un tableau 1D
            // pour eviter de mettre deux fois la meme arete dans la priority queue
            // index = row * (N - 2) - t(row - 1) + col - 1     où t(a) = a*(a+1) / 2
            //    e0  e1  e2
            //  e0    x   x
            //  e1        x
            //  e2
            // Il y a surement moyen de faire quelquechose de plus intelligent...
            int edgeProcessedSize = ind(numVertices - 1, numVertices - 1, numVertices) + 1;
            std::vector<bool> edgeProcessed(edgeProcessedSize, false);

            // parcours des aretes
            uint edgeProcessedInd;
            double edgeError;
            Vector3 p = Vector3::Zero();
            int j;
#pragma omp parallel for private(j, edgeProcessedInd, edgeError, p)
            for (unsigned int i = 0; i < numTriangles; i++)
            {
                const Face_ptr& f = m_dcel->m_face.at( i );
                HalfEdge_ptr h = f->HE();
                for (j = 0; j < 3; j++)
                {
                    const Vertex_ptr& vs = h->V();
                    const Vertex_ptr& vt = h->Next()->V();
                    if (vs->idx > vt->idx)//
                        continue;//
                    edgeProcessedInd = ind(min((vs->idx).getValue(), (vt->idx).getValue()),
                                           max((vs->idx).getValue(), (vt->idx).getValue()), numVertices);
                    if (!edgeProcessed[edgeProcessedInd])
                    {
                        edgeError = computeEdgeError(f->HE()->idx, p);

#pragma omp critical
                        {
                            edgeProcessed[edgeProcessedInd] = true;
                            pQueue.insert(PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, edgeError, p));
                        }
                    }
                    h = h->Next();
                }
            }

            return pQueue;
        }
        */

        template <class Primitive>
        PriorityQueue ProgressiveMesh<Primitive>::constructPriorityQueue()
        {
            PriorityQueue pQueue = PriorityQueue();

            const uint numTriangles = m_dcel->m_face.size();
            const uint numVertices = m_dcel->m_vertex.size();

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
                    if (vs->idx > vt->idx)
                        continue;
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

        template <class Primitive>
        void ProgressiveMesh<Primitive>::updatePriorityQueue(PriorityQueue &pQueue, Index vsIndex, Index vtIndex)
        {
            // we delete of the priority queue all the edge containing vs_id or vt_id
            pQueue.removeEdges(vsIndex);
            pQueue.removeEdges(vtIndex);

            double edgeError;
            Vector3 p = Vector3::Zero();
            Index vIndex;

            //VHEIterator vsHEIt = VHEIterator(m_dcel->m_vertex[vsIndex]);
            //HalfEdgeList adjHE = vsHEIt.list();

            // Adding an edge
            Vertex_ptr vs = m_dcel->m_vertex[vsIndex];
            HalfEdge_ptr h_start = vs->HE();
            edgeError = computeEdgeError(h_start->idx, p);
            vIndex = h_start->Next()->V()->idx;
            pQueue.insert(PriorityQueue::PriorityQueueData(vsIndex, vIndex, h_start->idx, h_start->F()->idx, edgeError, p));

            // Adding the other edges around vs
            HalfEdge_ptr h = h_start->Twin()->Next();
            // TODO if there is a hole
            while(h != h_start)
            {
                edgeError = computeEdgeError(h->idx, p);
                vIndex = h->Next()->V()->idx;

                pQueue.insert(PriorityQueue::PriorityQueueData(vsIndex, vIndex, h->idx, h->F()->idx, edgeError, p));

                h = h->Twin()->Next();
                CORE_ASSERT(h->V()->idx == vsIndex, "Invalid reference vertex");
            }
            //pQueue.display();
        }

        //--------------------------------------------------

        template <class Primitive>
        bool ProgressiveMesh<Primitive>::isEcolPossible(Index halfEdgeIndex, Vector3 pResult)
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

        template <class Primitive>
        std::vector<ProgressiveMeshData> ProgressiveMesh<Primitive>::constructM0(int targetNbFaces, int &nbNoFrVSplit)
        {
            uint nbPMData = 0;

            //m_pmdata = new ProgressiveMeshData[nbFullEdges];
            std::vector<ProgressiveMeshData> pmdata;
            // TODO ameliorer prédictopn nbr de VSPlit
            pmdata.reserve(targetNbFaces);

            LOG(logINFO) << "Computing Faces Quadrics";
            computeFacesQuadrics();

            LOG(logINFO) << "Computing Priority Queue";
            PriorityQueue pQueue = constructPriorityQueue();
            PriorityQueue::PriorityQueueData d;

            LOG(logINFO) << "Collapsing...";
            // while we do not have 'targetNbFaces' faces
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
            delete[](m_quadrics);

            m_quadrics = nullptr;

            LOG(logINFO) << "Collapsing done";

            return pmdata;
        }

        //--------------------------------------------------

        template <class Primitive>
        void ProgressiveMesh<Primitive>::vsplit(ProgressiveMeshData pmData)
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

        template <class Primitive>
        void ProgressiveMesh<Primitive>::ecol(ProgressiveMeshData pmData)
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

