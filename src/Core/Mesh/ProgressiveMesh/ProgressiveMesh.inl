#include "ProgressiveMesh.hpp"

#include <Core/RaCore.hpp>

#include <Core/Log/Log.hpp>

#include <Core/Mesh/Wrapper/Convert.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
//#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
#include <Core/Mesh/DCEL/Operations/VertexSplit.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

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
            m_em = ErrorMetric();
            m_nb_faces = mesh->m_triangles.size();
            m_nb_vertices = mesh->m_vertices.size();

            convert(*mesh, *m_dcel);

            m_mean_edge_length = Ra::Core::MeshUtils::getMeanEdgeLength(*mesh);
            m_scale = 0.0;
            m_ring_size = 0;
            m_weight_per_vertex = 0;
            m_primitive_update = 0;
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

        template <class ErrorMetric>
        inline ErrorMetric ProgressiveMesh<ErrorMetric>::getEM()
        {
            return m_em;
        }

        //------------------------------------------------

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::computeBoundingBoxSize(Scalar &min_x, Scalar &max_x, Scalar &min_y, Scalar &max_y, Scalar &min_z, Scalar &max_z)
        {
            min_x = max_x = m_dcel->m_vertex[0]->P().x();
            min_y = max_y = m_dcel->m_vertex[0]->P().y();
            min_z = max_z = m_dcel->m_vertex[0]->P().z();
            for (unsigned int i = 0; i < m_dcel->m_vertex.size(); i++)
            {
                if (m_dcel->m_vertex[i]->P().x() < min_x) min_x = m_dcel->m_vertex[i]->P().x();
                if (m_dcel->m_vertex[i]->P().x() > max_x) max_x = m_dcel->m_vertex[i]->P().x();
                if (m_dcel->m_vertex[i]->P().y() < min_y) min_y = m_dcel->m_vertex[i]->P().y();
                if (m_dcel->m_vertex[i]->P().y() > max_y) max_y = m_dcel->m_vertex[i]->P().y();
                if (m_dcel->m_vertex[i]->P().z() < min_z) min_z = m_dcel->m_vertex[i]->P().z();
                if (m_dcel->m_vertex[i]->P().z() > max_z) max_z = m_dcel->m_vertex[i]->P().z();
            }
            Vector3 size = Vector3(max_x-min_x, max_y-min_y, max_z-min_z);
            //Vector3 center = Vector3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
            m_bbox_size = size.norm();
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::scaleMesh()
        {
            Scalar min_x, max_x, min_y, max_y, min_z, max_z;
            computeBoundingBoxSize(min_x, max_x, min_y, max_y, min_z, max_z);
            Scalar maxX = std::max(std::abs(max_x), std::abs(min_x));
            Scalar maxY = std::max(std::abs(max_y), std::abs(min_y));
            Scalar maxZ = std::max(std::abs(max_z), std::abs(min_z));
            // TODO parallelization
            for (unsigned int i = 0; i < m_dcel->m_vertex.size(); i++)
            {
                m_dcel->m_vertex[i]->P().x() = m_dcel->m_vertex[i]->P().x() / maxX;
                m_dcel->m_vertex[i]->P().y() = m_dcel->m_vertex[i]->P().y() / maxY;
                m_dcel->m_vertex[i]->P().z() = m_dcel->m_vertex[i]->P().z() / maxZ;
            }
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::computeMeanEdgeLength()
        {
            Scalar mean_edge_length = 0.0;
            unsigned int nb_edges = 0;
            // TODO parallelization
            for (unsigned int i = 0; i < m_dcel->m_face.size(); i++)
            {
                Face_ptr f = m_dcel->m_face[i];
                if (f->HE() == NULL) continue;
                HalfEdge_ptr he = f->HE();
                mean_edge_length += (he->V()->P() - he->Next()->V()->P()).norm();
                nb_edges++;
                he = he->Next();
                mean_edge_length += (he->V()->P() - he->Next()->V()->P()).norm();
                nb_edges++;
                he = he->Next();
                mean_edge_length += (he->V()->P() - he->Next()->V()->P()).norm();
                nb_edges++;
            }
            mean_edge_length /= nb_edges;
            m_mean_edge_length = mean_edge_length;
        }

        //-----------------------------------------------------

        template<class ErrorMetric>
        typename ErrorMetric::Primitive ProgressiveMesh<ErrorMetric>::combine(const std::vector<Primitive>& primitives, const std::vector<Scalar>& weightsWedgeAngles)
        {
            std::vector<Scalar> weights;
            weights.reserve(primitives.size());
            Scalar normalizing_weight_factor = 1.0;

            if (m_weight_per_vertex == 0) // equal
            {
                for (unsigned int i = 0; i < primitives.size(); i++)
                {
                    //weights.push_back(1.0/primitives.size());
                    weights.push_back(1.0);
                }
                normalizing_weight_factor = primitives.size();
            }
            else if (m_weight_per_vertex == 1) // wedge angle
            {
                Scalar sumWedgeAngles = 0.0;
                for (unsigned int i = 0; i < primitives.size(); i++)
                {
                    sumWedgeAngles += weightsWedgeAngles[i];
                }
                normalizing_weight_factor = sumWedgeAngles;
                for (unsigned int i = 0; i < primitives.size(); i++)
                {
                    //weights.push_back(weightsWedgeAngles[i] / Scalar(sumWedgeAngles));
                    weights.push_back(weightsWedgeAngles[i]);
                }
            }
            return m_em.combine(primitives, weights, normalizing_weight_factor);
        }

        //-----------------------------------------------------

        template<class ErrorMetric>
        typename ErrorMetric::Primitive ProgressiveMesh<ErrorMetric>::computeVertexPrimitive(Index vertexIndex)
        {
            Primitive q;
            m_em.generateVertexPrimitive(q, m_dcel->m_vertex[vertexIndex], *m_dcel, m_scale, m_ring_size);
            return q;
        }

        template<class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::computeVerticesPrimitives()
        {
            const uint numVertices = m_dcel->m_vertex.size();
            const uint numFaces = m_dcel->m_face.size();

            m_primitives_he.clear();
            m_primitives_he.reserve(numFaces*3);
            m_primitives_v.clear();
            m_primitives_v.reserve(numVertices);

            Primitive q;
            for (uint v = 0; v < numVertices; ++v)
            {
                q = computeVertexPrimitive(v);
                m_primitives_v.push_back(q);
            }
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::updateVerticesPrimitives(Index vsIndex, HalfEdge_ptr he)
        {
            // We go all over the faces which contain vsIndex
            VVIterator vvIt = VVIterator(m_dcel->m_vertex[vsIndex]);
            VertexList adjVertices = vvIt.list();

            if (m_primitive_update == 0) // re-calcul
            {
                for (uint t = 0; t < adjVertices.size(); ++t)
                {
                    Primitive q;
                    m_em.generateVertexPrimitive(q, adjVertices[t], *m_dcel, m_scale, m_ring_size);
                    m_primitives_v[adjVertices[t]->idx] = q;
                }
            }
            else if (m_primitive_update == 1) // no update
            {
                m_primitives_v[vsIndex] = m_primitives_he[he->idx];
            }
        }

        template <class ErrorMetric>
        void ProgressiveMesh<ErrorMetric>::updateVerticesPrimitives(Index vsIndex, HalfEdge_ptr he, Vector3 v0, Vector3 v1, Index v0Ind, Index v1Ind, std::ofstream &file)
        {
            // We go all over the faces which contain vsIndex
            VVIterator vvIt = VVIterator(m_dcel->m_vertex[vsIndex]);
            VertexList adjVertices = vvIt.list();

            if (m_primitive_update == 0) // re-calcul
            {
                for (uint t = 0; t < adjVertices.size(); ++t)
                {
                    Primitive q;
                    m_em.generateVertexPrimitive(q, adjVertices[t], *m_dcel, m_scale, m_ring_size);
                    m_primitives_v[adjVertices[t]->idx] = q;
                }
            }
            else if (m_primitive_update == 1) // no update
            {
                Scalar vsv0 = (m_dcel->m_vertex[vsIndex]->P() - v0).norm();
                Scalar vsv1 = (m_dcel->m_vertex[vsIndex]->P() - v1).norm();
                Scalar vsv01 = (m_dcel->m_vertex[vsIndex]->P() - ((v0 + v1)/2.0)).norm();
                if (vsv0 < vsv1 && vsv0 < vsv01)
                {
                    m_primitives_v[vsIndex] = m_primitives_v[v0Ind];
                    file << "0\n";
                }
                else if (vsv1 < vsv0 && vsv1 < vsv01)
                {
                    m_primitives_v[vsIndex] = m_primitives_v[v1Ind];
                    file << "1\n";
                }
                else
                {
                    m_primitives_v[vsIndex] = m_primitives_he[he->idx];
                    file << "0.5\n";
                }
            }
        }

        //-----------------------------------------------------

        template <class ErrorMetric>
        typename ErrorMetric::Primitive ProgressiveMesh<ErrorMetric>::computeEdgePrimitive(Index halfEdgeIndex)
        {
            std::vector<Primitive> primitives;
            primitives.reserve(2);
            std::vector<Scalar> weights;
            weights.reserve(2);
            int v0Idx = m_dcel->m_halfedge[halfEdgeIndex]->V()->idx;
            int v1Idx = m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->idx;
            primitives.push_back(m_primitives_v[v0Idx]);
            primitives.push_back(m_primitives_v[v1Idx]);
            weights.push_back(0.5);
            weights.push_back(0.5);
            return combine(primitives, weights);
        }

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::computeEdgeError(Index halfEdgeIndex, Vector3 &pResult, Primitive &q)
        {
            Vector3 vs = m_dcel->m_halfedge[halfEdgeIndex]->V()->P();
            Vector3 vt = m_dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();
            q = computeEdgePrimitive(halfEdgeIndex);
            Scalar error = m_em.computeError(q, vs, vt, pResult);
            return error;
        }

        //-----------------------------------------------------

        template <class ErrorMetric>
        Scalar ProgressiveMesh<ErrorMetric>::getWedgeAngle(Index faceIndex, Index vIndex)
        {
            Scalar wedgeAngle;
            Face_ptr face = m_dcel->m_face[faceIndex];
            Vertex_ptr v = m_dcel->m_vertex[vIndex];

            HalfEdge_ptr he = face->HE();
            for (int i = 0; i < 3; i++)
            {
                if (he->V() == v)
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

        //--------------------------------------------------

        template <class ErrorMetric>
        PriorityQueue ProgressiveMesh<ErrorMetric>::constructPriorityQueue()
        {
            PriorityQueue pQueue = PriorityQueue();
            const uint numTriangles = m_dcel->m_face.size();
            //pQueue.reserve(numTriangles*3 / 2);

            // process through edges
            double edgeError;
            Vector3 p = Vector3::Zero();
            int j;

//#pragma omp parallel for private(j, edgeError, p)
            for (unsigned int i = 0; i < numTriangles; i++)
            {
                const Face_ptr& f = m_dcel->m_face.at( i );
                HalfEdge_ptr h = f->HE();
                for (j = 0; j < 3; j++)
                {
                    const Vertex_ptr& vs = h->V();
                    const Vertex_ptr& vt = h->Next()->V();

                    // To prevent adding twice the same edge
                    if (vs->idx > vt->idx)
                    {
                        h = h->Next();
                        continue;
                    }

                    Primitive q;
                    edgeError = computeEdgeError(h->idx, p, q);
                    m_primitives_he[h->idx] = q;

//#pragma omp critical
                    {
                        pQueue.insert(PriorityQueue::PriorityQueueData(vs->idx, vt->idx, h->idx, i, edgeError, p));
                    }
                    h = h->Next();
                }
            }
            //pQueue.display();
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

                Primitive q;
                edgeError = computeEdgeError(he->idx, p, q);
                m_primitives_he[he->idx] = q;

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
            {
                LOG(logINFO) << "The edge " << he->V()->idx << ", " << he->Next()->V()->idx << " in face " << he->F()->idx << " is not collapsable for now : T-Intersection";
                hasTIntersection = true;
                return false;
            }

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
                    Vector3 f_n = Geometry::triangleNormal(v->P(), v1->P(), v2->P());
                    Scalar fpnDotFn = fp_n.dot(f_n);
                    if (fpnDotFn < -0.5)
                    {
                        isFlipped = true;
                        LOG(logINFO) << "The edge " << he->V()->idx << ", " << he->Next()->V()->idx << " in face " << he->F()->idx << " is not collapsable for now : Flipped face";
                        return false;
                        break;
                    }
                }
            }

            return ((!hasTIntersection) && (!isFlipped));


            //return !hasTIntersection;
        }

        //--------------------------------------------------

        template <class ErrorMetric>
        std::vector<ProgressiveMeshData> ProgressiveMesh<ErrorMetric>::constructM0(int targetNbFaces, int &nbNoFrVSplit, int primitiveUpdate, Scalar scale, int weightPerVertex, std::ofstream &file)
        {
            uint nbPMData = 0;
            m_scale = scale;
            m_weight_per_vertex = weightPerVertex;
            m_primitive_update = primitiveUpdate;
            m_ring_size = std::floor((m_scale/m_mean_edge_length) + 1);
            LOG(logINFO) << "Ring Size = " << m_ring_size << "...";
            LOG(logINFO) << "Scale = " << m_scale << "...";

            std::vector<ProgressiveMeshData> pmdata;
            pmdata.reserve(targetNbFaces);

            LOG(logINFO) << "Computing Vertices Primitives...";
            computeVerticesPrimitives();

            LOG(logINFO) << "Computing Priority Queue...";
            PriorityQueue pQueue = constructPriorityQueue();
            PriorityQueue::PriorityQueueData d;

            LOG(logINFO) << "Collapsing...";
            ProgressiveMeshData data;

            while (m_nb_faces > targetNbFaces)
            {
                if (pQueue.empty()) break;
                d = pQueue.top();

                HalfEdge_ptr he = m_dcel->m_halfedge[d.m_edge_id];

                // TODO !
                if (!isEcolPossible(he->idx, d.m_p_result))
                    continue;

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

//#ifdef CORE_DEBUG
                data.setError(d.m_err);
                data.setPResult(d.m_p_result);
                data.setQCenter(m_primitives_he[he->idx].center());
                data.setQRadius(m_primitives_he[he->idx].radius());
                data.setQ1Center(m_primitives_v[d.m_vs_id].center());
                data.setQ1Radius(m_primitives_v[d.m_vs_id].radius());
                data.setQ2Center(m_primitives_v[d.m_vt_id].center());
                data.setQ2Radius(m_primitives_v[d.m_vt_id].radius());
                data.setVs((m_dcel->m_vertex[d.m_vs_id])->P());
                data.setVt((m_dcel->m_vertex[d.m_vt_id])->P());
                data.setGradientQ1(m_primitives_v[d.m_vs_id].primitiveGradient(data.getVs()));
                data.setGradientQ2(m_primitives_v[d.m_vt_id].primitiveGradient(data.getVt()));

                std::vector<ProgressiveMeshData::DataPerEdgeColor> err_per_edge = pQueue.copyToVector(m_dcel->m_halfedge.size(), *m_dcel);
                data.setErrorPerEdge(err_per_edge);
//#endif

                //
                Vector3 v0 = m_dcel->m_vertex[d.m_vs_id]->P();
                Vector3 v1 = m_dcel->m_vertex[d.m_vt_id]->P();
                //

                DcelOperations::edgeCollapse(*m_dcel, d.m_edge_id, d.m_p_result, true, data);

                //updateVerticesPrimitives(d.m_vs_id, he);
                updateVerticesPrimitives(d.m_vs_id, he, v0, v1, d.m_vs_id, d.m_vt_id, file);
                updatePriorityQueue(pQueue, d.m_vs_id, d.m_vt_id);

                pmdata.push_back(data);

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

            DcelOperations::edgeCollapse(*m_dcel, pmData);
        }
    }
}

