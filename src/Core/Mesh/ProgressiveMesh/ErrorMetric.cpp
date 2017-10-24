#include <Core/Mesh/ProgressiveMesh/ErrorMetric.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

#include <Core/Mesh/DCEL/Iterator/Edge/EFIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FFIterator.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Core
    {

        //---------------------------------------------------

        QuadricErrorMetric::QuadricErrorMetric()
        {
        }

        QuadricErrorMetric::QuadricErrorMetric(Scalar scale)
        {
            m_param.scale = scale;
        }

        bool QuadricErrorMetric::isPlanarEdge(Index halfEdgeIndex, Dcel* dcel)
        {
            bool planar = true;

            EFIterator efIt = EFIterator(dcel->m_halfedge[halfEdgeIndex]);
            FaceList adjFaces = efIt.list();

            Vector3 p0 = adjFaces[0]->HE()->V()->P();
            Vector3 q0 = adjFaces[0]->HE()->Next()->V()->P();
            Vector3 r0 = adjFaces[0]->HE()->Next()->Next()->V()->P();
            Vector3 n0 = Geometry::triangleNormal(p0,q0,r0);
            Scalar norm0 = n0.norm();
            Vector3 p, q, r, ni;
            Scalar normi;

            for (uint i = 1; i < adjFaces.size() && planar; i++)
            {
                p = adjFaces[i]->HE()->V()->P();
                q = adjFaces[i]->HE()->Next()->V()->P();
                r = adjFaces[i]->HE()->Next()->Next()->V()->P();
                ni = Geometry::triangleNormal(p,q,r);
                normi = ni.norm();
                planar = (std::abs(n0.dot(ni)) == norm0 * normi);
            }

            return planar;
        }

        bool QuadricErrorMetric::isPlanarEdge2(Index halfEdgeIndex, Dcel* dcel, Index &vsIndex, Index &vtIndex)
        {
            bool planarVs = true;
            bool planarVt = true;

            VFIterator vsIt = VFIterator(dcel->m_halfedge[halfEdgeIndex]->V());
            FaceList adjFacesVs = vsIt.list();

            Vector3 p0 = adjFacesVs[0]->HE()->V()->P();
            Vector3 q0 = adjFacesVs[0]->HE()->Next()->V()->P();
            Vector3 r0 = adjFacesVs[0]->HE()->Next()->Next()->V()->P();
            Vector3 n0 = Geometry::triangleNormal(p0,q0,r0);
            Scalar norm0 = n0.norm();
            Vector3 p, q, r, ni;
            Scalar normi;

            for (uint i = 1; i < adjFacesVs.size() && planarVs; i++)
            {
                p = adjFacesVs[i]->HE()->V()->P();
                q = adjFacesVs[i]->HE()->Next()->V()->P();
                r = adjFacesVs[i]->HE()->Next()->Next()->V()->P();
                ni = Geometry::triangleNormal(p,q,r);
                normi = ni.norm();
                planarVs = (std::abs(n0.dot(ni)) == norm0 * normi);
            }

            VFIterator vtIt = VFIterator(dcel->m_halfedge[halfEdgeIndex]->Next()->V());
            FaceList adjFacesVt = vtIt.list();

            p0 = adjFacesVt[0]->HE()->V()->P();
            q0 = adjFacesVt[0]->HE()->Next()->V()->P();
            r0 = adjFacesVt[0]->HE()->Next()->Next()->V()->P();
            n0 = Geometry::triangleNormal(p0,q0,r0);
            norm0 = n0.norm();

            for (uint i = 1; i < adjFacesVt.size() && planarVt; i++)
            {
                p = adjFacesVt[i]->HE()->V()->P();
                q = adjFacesVt[i]->HE()->Next()->V()->P();
                r = adjFacesVt[i]->HE()->Next()->Next()->V()->P();
                ni = Geometry::triangleNormal(p,q,r);
                normi = ni.norm();
                planarVt = (std::abs(n0.dot(ni)) == norm0 * normi);
            }

            if (planarVs)
            {
                vsIndex = -1;
            }
            else
            {
                vsIndex = dcel->m_halfedge[halfEdgeIndex]->V()->idx;
            }

            if (planarVt)
            {
                vtIndex = -1;
            }
            else
            {
                vtIndex = dcel->m_halfedge[halfEdgeIndex]->Next()->V()->idx;
            }

            return (planarVs && planarVt);
        }

        Scalar QuadricErrorMetric::computeGeometricError(const Primitive& q, const Primitive::Vector& p)
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

        //Scalar QuadricErrorMetric::computeError(const Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult)
        Scalar QuadricErrorMetric::computeError(const Primitive& q, Index halfEdgeIndex, Vector3& pResult, Dcel* dcel)
        {
            Vector3 vs = dcel->m_halfedge[halfEdgeIndex]->V()->P();
            Vector3 vt = dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();

            Scalar error;

            // on cherche v_result
            // A v_result = -b		avec A = nn^T
            //							 b = dn
            Primitive::Matrix AInverse = q.getA().inverse();
            Primitive::Vector result;

            Scalar det = q.getA().determinant();
            if (det > 0.0001)
            {
                result = -AInverse * q.getB();
                error = std::abs(computeGeometricError(q, result));
            }
            else //matrix non inversible
            {
                Primitive::Vector p1  = vs;
                Primitive::Vector p2  = vt;
                Primitive::Vector p12 = (p1 + p2) / 2.0;
                Scalar p12_error = std::abs(computeGeometricError(q, p12));
                bool planar = isPlanarEdge(halfEdgeIndex, dcel);
                if (planar)
                {
                    result = p12;
                    error = p12_error;
                }
                else
                {
                    Scalar p1_error     = std::abs(computeGeometricError(q, p1));
                    Scalar p2_error     = std::abs(computeGeometricError(q, p2));
                    error = p1_error;
                    if (p2_error < error && p12_error > p2_error)
                    {
                        result = p2;
                        error = p2_error;
                    }
                    else if (p12_error < error && p2_error > p12_error)
                    {
                        result = p12;
                        error = p12_error;
                    }
                    else
                    {
                        result = p1;
                    }
                }


//                Primitive::Vector p1  = vs;
//                Primitive::Vector p2  = vt;
//                Primitive::Vector p12 = (p1 + p2) / 2.0;

//                Scalar p1_error     = std::abs(computeGeometricError(q, p1));
//                Scalar p2_error     = std::abs(computeGeometricError(q, p2));
//                Scalar p12_error    = std::abs(computeGeometricError(q, p12));

//                error = p1_error;
//                Primitive::Vector p = p1;
//                if (p2_error < error && p12_error > p2_error)
//                {
//                    p = p2;
//                    result = p;
//                    error = p2_error;
//                }
//                else if (p12_error < error && p2_error > p12_error)
//                {
//                    p = p12;
//                    result = p;
//                    error = p12_error;
//                }
//                else
//                {
//                    result = p;
//                }
            }
            pResult = Vector3(result.x(), result.y(), result.z());
            return error;
        }

//        Scalar QuadricErrorMetric::computeError(const Primitive& q, Index halfEdgeIndex, Vector3& pResult, Dcel* dcel)
//        {
//            Vector3 vs = dcel->m_halfedge[halfEdgeIndex]->V()->P();
//            Vector3 vt = dcel->m_halfedge[halfEdgeIndex]->Next()->V()->P();

//            Scalar error;
//            Primitive::Vector result;

//            Primitive::Vector p1  = vs;
//            Primitive::Vector p2  = vt;
//            Primitive::Vector p12 = (p1 + p2) / 2.0;
//            Scalar p12_error = std::abs(computeGeometricError(q, p12));
//            bool planar = isPlanarEdge(halfEdgeIndex, dcel);
//            if (planar)
//            {
//                result = p12;
//                error = p12_error;
//            }

//            else
//            {
//                // on cherche v_result
//                // A v_result = -b		avec A = nn^T
//                //							 b = dn
//                Primitive::Matrix AInverse = q.getA().inverse();


//                Scalar det = q.getA().determinant();
//                if (det > 0.0001)
//                {
//                    result = -AInverse * q.getB();
//                    error = std::abs(computeGeometricError(q, result));
//                }
//                else //matrix non inversible
//                {

//                    Scalar p1_error     = std::abs(computeGeometricError(q, p1));
//                    Scalar p2_error     = std::abs(computeGeometricError(q, p2));
//                    error = p1_error;
//                    if (p2_error < error && p12_error > p2_error)
//                    {
//                        result = p2;
//                        error = p2_error;
//                    }

//                    else
//                    {
//                        result = p1;
//                    }
//                }
//            }

//            pResult = Vector3(result.x(), result.y(), result.z());
//            return error;
//        }

        void QuadricErrorMetric::generateFacePrimitive(Primitive &q, const Face_ptr f, Dcel &dcel)
        {
            Vertex_ptr v0 = f->HE()->V();
            Vertex_ptr v1 = f->HE()->Next()->V();
            Vertex_ptr v2 = f->HE()->Next()->Next()->V();

            Primitive::Vector n = Geometry::triangleNormal(v0->P(), v1->P(), v2->P());
            q = Primitive(n, -n.dot(v0->P()));
        }

    }
}


