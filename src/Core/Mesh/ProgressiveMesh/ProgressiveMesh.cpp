#include "ProgressiveMesh.hpp"

#include <Core/RaCore.hpp>

#include <Core/Mesh/Wrapper/Convert.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define triang(x) (((x) * ((x) + 1)) / 2)
#define ind(row, col, n) ((row) * ((n) - 2) - triang((row) - 1) + (col) - 1)

namespace Ra
{
    namespace Core
    {

        ProgressiveMesh::ProgressiveMesh(TriangleMesh* mesh)
        {
            m_mesh = mesh;
            m_quadrics = new Quadric[m_mesh->m_triangles.size()]();
            convert(*m_mesh, *m_dcel);
        }

        //------------------------------------------------

        void ProgressiveMesh::computeFacesQuadrics()
        {
            const uint numTriangles = m_mesh->m_triangles.size();
            for (uint t = 0; t < numTriangles; ++t)
            {
                std::array<Vector3, 3> triVerts;
                MeshUtils::getTriangleVertices(*m_mesh, t, triVerts);

                Vector3 n = MeshUtils::getTriangleNormal(*m_mesh, t);
                Vector3 v0 = triVerts[0];
                double ndotp = -n.dot(v0);

                Quadric q;
                q.compute(n, ndotp);

                m_quadrics[t] = q;
            }
        }

        Ra::Core::Quadric ProgressiveMesh::computeEdgeQuadric(Index edgeIndex)
        {
            FullEdge_ptr edge = m_dcel->m_fulledge[edgeIndex];
            Vertex_ptr vs = edge->V(0);
            Vertex_ptr vt = edge->V(1);

            // We go all over the faces which contain vs and vt
            // We add the quadrics of all the faces

            // Beware : particular cases for going through all the adjacent faces
            // = : border
            /*
                                  A                                     A
                        \\      /   \       //       |        \\      /   \       //
                         \\    /     \     //        |         \\    /     \     //
                          \\  /       \   //         |          \\  /       \   //
                           \\/         \ //          |           \\/         \ //
                            V1----------V2           |            V1==========V2
                           //\         / \\          |
                          //  \       /   \\         |
                         //    \     /     \\        |
                        //      \   /       \\       |
                                  B
              */

            HalfEdge_ptr halfEdge0 = edge->HE(0);
            HalfEdge_ptr halfEdge1 = edge->HE(1);
            HalfEdge_ptr halfEdge0Next = halfEdge0->Next();
            HalfEdge_ptr halfEdge1Prev = halfEdge1->Prev();

            while (halfEdge0Next->F() != halfEdge1Prev->F())
            {


            }

            /*
            Quadric q0;
            for (unsigned int i = 0; i < f_id_v0.size(); i++)
            {
                Quadric* qf = getQuadricPtr(f_id_v0[i]);
                q0.setC(q0.getC() + qf->getC());
                q0.setb1(q0.getb1() + qf->getb1());
                q0.setfc(q0.getfc() + qf->getfc());
            }

            Quadric q1 = Quadric(nb_attrib * f_id_v1.size());
            for (unsigned int i = 0; i < f_id_v1.size(); i++)
            {
                Quadric* qf = getQuadricPtr(f_id_v1[i]);
                q1.setC(q1.getC() + qf->getC());
                q1.setb1(q1.getb1() + qf->getb1());
                q1.setfc(q1.getfc() + qf->getfc());
            }

            return q0 + q1;
            */
        }

        //-----------------------------------------------------

        double ProgressiveMesh::computeEdgeError(Index edgeIndex, Vector3 &p_result)
        {
            Quadric q = computeEdgeQuadric(edgeIndex);

            // on cherche v_result
            // A v_result = -b		avec A = nn^T
            //							 b = dn
            Matrix3 A_inverse = q.getA().inverse();
            p_result = -A_inverse * q.getB();
            double error = computeGeometricError(p_result, q);

            // Si par hasard on ne peut pas inverser A (det == 0)
            /*
            Vector3 p1 = vs->P();
            Vector3 p2 = vt->P();
            Vector3 p12 = (p1 + p2) / 2.0;

            double error;
            double p1_error = computeGeometricError(p1, q);
            double p2_error = computeGeometricError(p2, q);
            double p12_error = computeGeometricError(p12, q);
            error = std::min({p1_error, p2_error, p12_error});
            */
            return error;
        }


        double ProgressiveMesh::computeGeometricError(const Vector3& p, Quadric q)
        {
            // Computing geometric error
            // v^T A v + 2 * b^T v + c
            Eigen::Matrix<Scalar, 1, 3> row_p = p.transpose();
            Eigen::Matrix<Scalar, 1, 3> row_b = q.getB().transpose();
            double error_a = row_p * q.getA() * p;
            double error_b = 2.0 * row_b * p;
            double error_c = q.getC();
            return (error_a + error_b + error_c);
        }

        //--------------------------------------------------

        PriorityQueue ProgressiveMesh::constructPriorityQueue()
        {
            PriorityQueue pQueue = PriorityQueue();

            const uint numTriangles = m_dcel->m_face.size();
            const uint numVertices = m_dcel->m_vertex.size();

            // matrice triangulaire infèrieure sans diagonale dans un tableau 1D
            // pour éviter de mettre deux fois la même arête dans la priority queue
            // index = row * (N - 2) - t(row - 1) + col - 1     où t(a) = a*(a+1) / 2
            //    e0  e1  e2
            //  e0    x   x
            //  e1        x
            //  e2
            // Il y a sûrement moyen de faire quelquechose de plus intelligent...
            int edgeProcessedSize = ind(numVertices - 1, numVertices - 1, numVertices) + 1;
            std::vector<bool> edgeProcessed(edgeProcessedSize, false);

            // parcourt des arêtes
            uint vsId, vtId, edgeProcessedInd;
            double edgeError;
            Vector3 p;
            for (unsigned int i = 0; i < numTriangles; i++)
            {
                const Face_ptr& f = m_dcel->m_face.at( i );
                for (int j = 0; j < 3; j++)
                {
                    const Vertex_ptr& vs = f->HE()->V();
                    const Vertex_ptr& vt = f->HE()->Next()->V();
                    vsId = vs->idx;
                    vtId = vt->idx;
                    edgeProcessedInd = ind(min(vsId, vtId), max(vsId, vtId), numVertices);
                    if (!edgeProcessed[edgeProcessedInd])
                    {
                        // l'erreur sur cette arête n'a pas été calculée
                        edgeError = computeEdgeError(f->HE()->FE()->idx, p);
                        edgeProcessed[edgeProcessedInd] = true;
                        pQueue.insert(PriorityQueue::PriorityQueueData(vsId, vtId, f->HE()->FE()->idx, i, edgeError, p));
                    }
                }
            }
            //p_queue.display();
            return pQueue;
        }

        void ProgressiveMesh::updatePriorityQueue(PriorityQueue &pQueue, int vsId, int vtId, int edgeId)
        {
            // on supprime de la file toutes les arêtes qui contiennent vs_id ou vt_id
            pQueue.removeEdges(vsId);
            if (vtId != -1)
                pQueue.removeEdges(vtId);

            // parcours des arêtes autour de edgeId




            //p_queue.display();
        }



        //--------------------------------------------------

        void ProgressiveMesh::constructM0(int targetNbFaces)
        {
            uint nbFaces = m_dcel->m_face.size();
            uint nbFullEdges = m_dcel->m_fulledge.size();
            uint nbVSplits = 0;

            m_vsplits = new VSplit[nbFullEdges];
            computeFacesQuadrics();

            PriorityQueue pQueue = constructPriorityQueue();
            PriorityQueue::PriorityQueueData d;

            // while we do not have 'targetNbFaces' faces
            while (nbFaces > targetNbFaces)
            {
                if (pQueue.empty()) break;
                d = pQueue.top();

                // if isEcolPossible !

                FullEdge_ptr fe = m_dcel->m_fulledge[d.m_edge_id];
                if (fe->HE(0) == NULL || fe->HE(1) == NULL) nbFaces -= 1;
                else nbFaces -= 2;

                //m_vsplits[nbVSplits] = DcelOperations::edgeCollapse(*m_dcel, d.m_edge_id);
                /*
                updateQuadrics();
                updatePriorityQueue(p_queue, vl_id, -1);//
                */
                nbVSplits++;
            }
            delete[](m_quadrics);
        }
    }
}
