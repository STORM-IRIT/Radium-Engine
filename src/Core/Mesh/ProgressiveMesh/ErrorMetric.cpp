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

        //---------------------------------------------------

        APSSErrorMetric::APSSErrorMetric()
        {
        }

        APSSErrorMetric::APSSErrorMetric(Scalar scale)
        {
            m_param.scale = scale;
        }

        Scalar APSSErrorMetric::computeGeometricError(const Primitive& q, const Primitive::Vector& p)
        {
            // Computing geometric error
            // v^T A v + 2 * b^T v + c
            /*
            Scalar uc = std::sqrt(q.getC());
            Vector3 ul = Vector3(q.getB().x(), q.getB().y(), q.getB().z()) / uc;
            Scalar uq = q.getB().w();
            Vector3 p2 = Vector3(p.x(), p.y(), p.z());
            return uc + p2.dot(ul) + uq * p2.squaredNorm();
            */

            Eigen::Matrix<Scalar, 1, 4> row_p = p.transpose();
            Eigen::Matrix<Scalar, 1, 4> row_b = q.getB().transpose();
            Scalar error_a = row_p * q.getA() * p;
            Scalar error_b = 2.0 * row_b * p;
            Scalar error_c = q.getC();
            return (error_a + error_b + error_c);

        }

        Scalar APSSErrorMetric::computeError(const Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult)
        {
            Scalar error;

            // on cherche v_result
            // A v_result = -b		avec A = nn^T
            //							 b = dn
            Primitive::Matrix AInverse = q.getA().inverse();
            Primitive::Vector vsPrimitiveType = Primitive::Vector(vs.x(), vs.y(), vs.z(), vs.norm() * vs.norm());
            Primitive::Vector vtPrimitiveType = Primitive::Vector(vt.x(), vt.y(), vt.z(), vt.norm() * vt.norm());
            Primitive::Vector result;

            Scalar det = q.getA().determinant();
            /*
            if (det > 0.0001)
            {
                result = -AInverse * q.getB();
                error = computeGeometricError(q, result);
            }
            else //matrix non inversible
            {
                Primitive::Vector p1  = vsPrimitiveType;
                Primitive::Vector p2  = vtPrimitiveType;
                Primitive::Vector p12 = (p1 + p2) / 2.0;

                Scalar p1_error     = computeGeometricError(q, p1);
                Scalar p2_error     = computeGeometricError(q, p2);
                Scalar p12_error    = computeGeometricError(q, p12);

                error = p1_error;
                Primitive::Vector p = p1;
                if (p2_error < error && p12_error > p2_error)
                {
                    p = p2;
                    result = p;
                    error = p2_error;
                }
                else if (p12_error < error && p2_error > p12_error)
                {
                    p = p12;
                    result = p;
                    error = p12_error;
                }
                else
                {
                    result = p;
                }
            }
            */
            Primitive::Vector p1  = vsPrimitiveType;
            Primitive::Vector p2  = vtPrimitiveType;
            Primitive::Vector p12 = Primitive::Vector((vs.x() + vt.x()) / 2.0,
                                                      (vs.y() + vt.y()) / 2.0,
                                                      (vs.z() + vt.z()) / 2.0,
                                                      (vs + vt).norm() * (vs + vt).norm());

            Scalar p1_error     = computeGeometricError(q, p1);
            Scalar p2_error     = computeGeometricError(q, p2);
            Scalar p12_error    = computeGeometricError(q, p12);
            result = p12;
            error = p12_error;

            pResult = Vector3(result.x(), result.y(), result.z());

            return error;
        }

        void APSSErrorMetric::generateFacePrimitive(Primitive &q, Face_ptr f, Dcel &dcel)
        {
            Vertex_ptr v0 = f->HE()->V();
            Vertex_ptr v1 = f->HE()->Next()->V();
            Vertex_ptr v2 = f->HE()->Next()->Next()->V();
            Vector3 p = (v0->P() + v1->P() + v2->P()) / 3.0;
            GrenaillePoint::VectorType pg = GrenaillePoint::VectorType(p.x(), p.y(), p.z());

            Fit1 fit;
            fit.setWeightFunc(WeightFunc());
            fit.init(pg);
            //fit.init(GrenaillePoint::VectorType::Zero()); //no more local

            GrenaillePoint::VectorType pgi;
            FFIterator ffIt = FFIterator(f);
            FaceList adjFaces = ffIt.list();
            for (uint i = 0; i < adjFaces.size(); i++)
            {
                Face_ptr fi = adjFaces[i];
                v0 = fi->HE()->V();
                v1 = fi->HE()->Next()->V();
                v2 = fi->HE()->Next()->Next()->V();
                p = (v0->P() + v1->P() + v2->P()) / 3.0;
                pgi = GrenaillePoint::VectorType(p.x(), p.y(), p.z());
                Vector3 n = Geometry::triangleNormal(v0->P(), v1->P(), v2->P());
                GrenaillePoint gpi(pgi, n);
                fit.addNeighbor(gpi);
            }
            fit.finalize();

            if (fit.isStable())
            {
                //fit.applyPrattNorm();

                Scalar uc = fit.m_uc; // + pg.dot(fit.m_ul); //switch local to global
                GrenaillePoint::VectorType ul = fit.m_ul;
                Scalar uq = fit.m_uq;

                // Test
                uc = uc / ul.norm();
                ul.normalize();

                Primitive::Vector n = Primitive::Vector(ul.x(),  //ul
                                                        ul.y(),
                                                        ul.z(),
                                                        0.0);     //uq
                Scalar s = uc;
                q = Primitive(n, s);

                GrenaillePoint::VectorType center = fit.center();
                Scalar radius = fit.radius();
                m_param.fits.push_back(fit);
            }
            else
            {
                CORE_ASSERT(fit.isStable(), "APSS FIT IS NOT STABLE");
            }
        }

        //---------------------------------------------------

        SimpleAPSSErrorMetric::SimpleAPSSErrorMetric()
        {
        }

        SimpleAPSSErrorMetric::SimpleAPSSErrorMetric(Scalar scale)
        {
            m_param.scale = scale;
        }

        Scalar SimpleAPSSErrorMetric::computeGeometricError(const Primitive& q, const Vector3& p)
        {

        }

        SimpleAPSSErrorMetric::Primitive SimpleAPSSErrorMetric::combine(const Primitive& a, const Primitive& b)
        {
            Primitive c = a;
            c.changeBasis(b.basisCenter());
            c.applyPrattNorm();

            c.setParameters((c.tau() + b.tau()), (c.eta() + b.eta()), (c.kappa() + b.kappa()));

            c.changeBasis(a.basisCenter());
            c.applyPrattNorm();
            return c;
        }

        Scalar SimpleAPSSErrorMetric::computeError(Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult)
        {
            GrenaillePoint::VectorType center = q.center();
            Scalar radius = q.radius();
            GrenaillePoint::VectorType p12 = (vs + vt) / 2.0;
            pResult = q.project(p12);

            return std::abs(q.potential(p12));

            // Projection of p12 on the sphere
            /*
            Vector3 pSphere = Vector3(p12.x() - center.x(),
                                      p12.y() - center.y(),
                                      p12.z() - center.z());

            Scalar pSphereLength = pSphere.norm();
            Vector3 qSphere = pSphere * (radius / pSphereLength);
            Vector3 rSphere = Vector3(qSphere.x() + center.x(),
                                      qSphere.y() + center.y(),
                                      qSphere.z() + center.z());
            pResult = rSphere;
            return (p12 - rSphere).norm();
            */
        }

        void SimpleAPSSErrorMetric::generateFacePrimitive(Primitive &q, Face_ptr f, Dcel &dcel, Scalar scale)
        {
            Vertex_ptr v0 = f->HE()->V();
            Vertex_ptr v1 = f->HE()->Next()->V();
            Vertex_ptr v2 = f->HE()->Next()->Next()->V();
            Vector3 p = (v0->P() + v1->P() + v2->P()) / 3.0;
            GrenaillePoint::VectorType pg = GrenaillePoint::VectorType(p.x(), p.y(), p.z());

            Fit1 fit;
            fit.setWeightFunc(WeightFunc(scale));
            fit.init(pg);
            //fit.init(GrenaillePoint::VectorType::Zero()); //no more local

            GrenaillePoint::VectorType pgi;
            FFIterator ffIt = FFIterator(f);
            FaceList adjFaces = ffIt.list();
            for (uint i = 0; i < adjFaces.size(); i++)
            {
                Face_ptr fi = adjFaces[i];
                v0 = fi->HE()->V();
                v1 = fi->HE()->Next()->V();
                v2 = fi->HE()->Next()->Next()->V();
                p = (v0->P() + v1->P() + v2->P()) / 3.0;
                pgi = GrenaillePoint::VectorType(p.x(), p.y(), p.z());
                Vector3 n = Geometry::triangleNormal(v0->P(), v1->P(), v2->P());
                GrenaillePoint::VectorType ng = GrenaillePoint::VectorType(n.x(), n.y(), n.z());
                GrenaillePoint gpi(pgi, ng);
                fit.addNeighbor(gpi);
            }
            fit.finalize();

            if (fit.getCurrentState() != UNDEFINED)
            {
                q = fit;
                Scalar uc = fit.m_uc;
                GrenaillePoint::VectorType ul = fit.m_ul;
                Scalar uq = fit.m_uq;
                m_param.fits.push_back(fit);
            }
            else
            {
                CORE_ASSERT(true, "APSS FIT IS NOT STABLE");
            }
        }

    }
}


