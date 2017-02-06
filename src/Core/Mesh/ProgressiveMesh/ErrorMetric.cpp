#include <Core/Mesh/ProgressiveMesh/ErrorMetric.hpp>

#include <Core/Geometry/Triangle/TriangleOperation.hpp>

#include <Core/Mesh/DCEL/Iterator/Edge/EFIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Face/FFIterator.hpp>
#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>
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

        Scalar QuadricErrorMetric::computeError(const Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult)
        {
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
                error = computeGeometricError(q, result);
            }
            else //matrix non inversible
            {
                Primitive::Vector p1  = vs;
                Primitive::Vector p2  = vt;
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
            pResult = Vector3(result.x(), result.y(), result.z());
            return error;
        }

        QuadricErrorMetric::Primitive QuadricErrorMetric::combine(const std::vector<Primitive>& p, const std::vector<Scalar>& weights, Scalar normalizing_weight_factor)
        {
            Primitive sum;

            for (unsigned int i = 0; i < p.size(); i++)
            {
                sum += p[i] * weights[i];
            }

            return sum;
        }

        void QuadricErrorMetric::generateVertexPrimitive(Primitive &q, Vertex_ptr v, Dcel &dcel, Scalar weight, int ringSize)
        {
            VFIterator vfIt = VFIterator(v);
            FaceList adjFaces = vfIt.list();

            for (unsigned int i = 0; i < adjFaces.size(); i++)
            {
                Face_ptr fi = adjFaces[i];
                Primitive qi;
                generateFacePrimitive(qi, fi, dcel, weight, ringSize);
                q += qi;
            }
        }

        void QuadricErrorMetric::generateFacePrimitive(Primitive &q, Face_ptr f, Dcel &dcel, Scalar weight, int ringSize)
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
                //q = fit;
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

        /*
        SimpleAPSSErrorMetric::Primitive SimpleAPSSErrorMetric::combine(const Primitive& a, const Primitive& b)
        {
            Primitive c = a;
            c.changeBasis(b.basisCenter());
            c.applyPrattNorm();

            //c.setParameters((c.tau() + b.tau())/Scalar(2), (c.eta() + b.eta()), (c.kappa() + b.kappa())/Scalar(2));
            c.setParameters((c.tau() + b.tau()), (c.eta() + b.eta()), (c.kappa() + b.kappa()));

            c.changeBasis(a.basisCenter());
            c.applyPrattNorm();
            return c;
        }
        */

        /*
        SimpleAPSSErrorMetric::Primitive SimpleAPSSErrorMetric::combine(const Primitive& a, const Scalar& a_weight, const Primitive& b, const Scalar& b_weight)
        {
            Primitive c;
            Scalar new_uc, new_uq;
            Vector3 new_ul, new_p;
            if (a.isPlane() && b.isPlane())
            {
                // PLAN
                new_ul  = a_weight * a.m_ul + b_weight * b.m_ul;
                new_uq  = 0.0;
                new_p   = a_weight * a.m_p + b_weight * b.m_p;
                new_uc  = a_weight * a.potential(new_p) + b_weight * b.potential(new_p);
            }
            else
            {
                // SPHERE
                new_uc  =   0.5*a_weight*b_weight*(a.m_ul.dot(b.m_ul)) +
                                    a_weight*a_weight*a.m_uc*b.m_uq +
                                    b_weight*b_weight*b.m_uc*a.m_uq -
                                    0.5*a_weight*b_weight*a.prattNorm()*b.prattNorm();
                new_ul  = a_weight*a.m_ul*b.m_uq + b_weight*b.m_ul*a.m_uq;
                new_uq  = a.m_uq*b.m_uq;
                new_p   = a_weight*a.m_p + b_weight*b.m_p;
            }

#ifdef CORE_DEBUG
            Scalar test = new_ul.squaredNorm() - Scalar(4.) * new_uc * new_uq;
            if (test > -0.00000000001 && test < 0.00000000001)
            {
                LOG(logINFO) << "ca merde ici : norm 0 \n";
            }
            if (std::abs(test) > 10000000000000)
            {
                LOG(logINFO) << "ca merde ici : norm inf \n";
            }
            if (test < 0)
            {
                LOG(logINFO) << "ca merde ici : norm negative \n";
            }
#endif
            c.setParameters(new_uc, new_ul, new_uq, new_p);
            return c;
        }
        */

        SimpleAPSSErrorMetric::Primitive SimpleAPSSErrorMetric::combine(const std::vector<Primitive>& p, const std::vector<Scalar>& weights, Scalar normalizing_weight_factor)
        {
            Scalar new_uc = 0.0;
            Scalar new_uq = 0.0;
            Vector3 new_ul = Vector3(0.0, 0.0, 0.0);
            Vector3 new_p = Vector3(0.0, 0.0, 0.0);

            // Determine if all primitives are planes
            bool arePlanes = true;
            for (unsigned int i = 0; i < p.size(); i++)
            {
                if (!p[i].isPlane())
                {
                    arePlanes = false;
                    break;
                }
            }

            Scalar sumWeights = 0.0;
            if (arePlanes) // PLANE
            {
                for (unsigned int i = 0; i < p.size(); i++)
                {
                    new_ul  += weights[i] * p[i].m_ul;
                    new_uq  = 0.0;
                    new_p   += weights[i] * p[i].m_p;
                    sumWeights += weights[i];
                }
                //
                new_ul /= normalizing_weight_factor;
                new_uq /= normalizing_weight_factor;
                new_p /= normalizing_weight_factor;
                //
                new_ul.normalize();
                for (unsigned int i = 0; i < p.size(); i++)
                {
                    new_uc  += weights[i] * p[i].potential(new_p);
                }
                new_uc /= normalizing_weight_factor;
            }
            else // SPHERE
            {
                // linear interpolation of parameters
                for (unsigned int i = 0; i < p.size(); i++)
                {
                    new_ul  += weights[i] * p[i].m_ul;
                    new_uq  += weights[i] * p[i].m_uq;
                    //new_uq  *= p[i].m_uq;
                    new_p   += weights[i] * p[i].m_p;
                    sumWeights += weights[i];
                }
                //
                new_ul /= normalizing_weight_factor;
                new_uq /= normalizing_weight_factor;
                new_p /= normalizing_weight_factor;
                //
                new_ul.normalize();
                for (unsigned int i = 0; i < p.size(); i++)
                {
                    new_uc  += weights[i] * p[i].potential(new_p);
                }
                new_uc /= normalizing_weight_factor;
                // TODO sometimes sqrt(-x)
                //new_ul *= sqrt(1.0 + 4.0 * new_uc * new_uq);
                new_ul *= sqrt(std::abs(1.0 + 4.0 * new_uc * new_uq));
            }
            CORE_ASSERT((sumWeights / normalizing_weight_factor) < 1.01 && (sumWeights / normalizing_weight_factor) > 0.99, "The sum of weights is not equal to 1");

            Primitive c;
            c.setParameters(new_uc, new_ul, new_uq, new_p);
            c.applyPrattNorm();

            CORE_ASSERT(!std::isnan(new_uc), "PRIMITIVE NAN : NOT OK");
            CORE_ASSERT(std::abs(new_ul.squaredNorm() - Scalar(4.) * new_uc * new_uq) > 0.00000000001, "PRIMITIVE 0 : NOT OK");
            CORE_ASSERT(!std::isnan(std::abs(new_ul.squaredNorm() - Scalar(4.) * new_uc * new_uq)), "PRIMITIVE WITH NORM ZERO : NOT OK");
            CORE_ASSERT(new_ul.squaredNorm() - Scalar(4.) * new_uc * new_uq > 0.0, "PRIMITIVE WITH NEGATIVE NORM : NOT OK");

            return c;
        }

        /*
        Scalar SimpleAPSSErrorMetric::computeError(Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult)
        {
            GrenaillePoint::VectorType p12 = (vs + vt) / 2.0;
            GrenaillePoint::VectorType p1 = vs;
            GrenaillePoint::VectorType p2 = vt;
            Scalar error;
            Scalar error12 = std::abs(q.potential(p12));
            Scalar error1 = std::abs(q.potential(p1));
            Scalar error2 = std::abs(q.potential(p2));
            if (error12 <= error1 && error12 <= error2)
            {
                pResult = q.project(p12);
                error = error12;
            }
            else if (error1 < error2)
            {
                pResult = q.project(p1);
                error = error1;
            }
            else
            {
                pResult = q.project(p2);
                error = error2;
            }

            error = std::max(std::max(error12, error1), error2);
            CORE_ASSERT(!std::isnan(std::abs(error)), "ERROR TOO HIGH");
            return error;
        }
        */

        Scalar SimpleAPSSErrorMetric::computeError(Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult)
        {
            // compute error as integral
            Scalar uc = 0.0;
            Vector3 ul = Vector3(0.5 * q.m_ul.x() + q.m_uq * vs.x(), 0.5 * q.m_ul.y() + q.m_uq * vs.y(), 0.5 * q.m_ul.z() + q.m_uq * vs.z());
            Scalar uq = q.m_uq / 3.0;
            Vector3 p = Vector3::Zero();

            Primitive q2;
            q2.setParameters(uc, ul, uq, p);
            q2.applyPrattNorm();

            Vector3 segment = vt - vs;

            // compute resulting vertex as usual
            GrenaillePoint::VectorType p12 = (vs + vt) / 2.0;
            GrenaillePoint::VectorType p1 = vs;
            GrenaillePoint::VectorType p2 = vt;
            Scalar error12 = std::abs(q.potential(p12));
            Scalar error1 = std::abs(q.potential(p1));
            Scalar error2 = std::abs(q.potential(p2));
            if (error12 <= error1 && error12 <= error2)
            {
                pResult = q.project(p12);
            }
            else if (error1 < error2)
            {
                pResult = q.project(p1);
            }
            else
            {
                pResult = q.project(p2);
            }

            return std::abs(q.potential(vs)) + std::abs(q2.potential(segment) * segment.norm());
        }


        void SimpleAPSSErrorMetric::generateVertexPrimitive(Primitive &q, Vertex_ptr v, Dcel &dcel, Scalar weight, int ringSize)
        {
            Vector3 p = v->P();
            GrenaillePoint::VectorType pg = GrenaillePoint::VectorType(p.x(), p.y(), p.z());
            GrenaillePoint::VectorType new_pg = GrenaillePoint::VectorType(p.x(), p.y(), p.z());

            Fit1 fit;
            fit.setWeightFunc(WeightFunc(weight)); // TODO weight func

            Scalar error;
            VVIterator vvIt = VVIterator(v);
            std::set<Vertex_ptr, VVIterator::compareVertexPtr> adjVerticesSet;
            vvIt.nRing(ringSize, adjVerticesSet);
            unsigned int nb_of_loop = 0;
            do {
                fit.init(new_pg);
                GrenaillePoint::VectorType pgi;
                std::set<Vertex_ptr, VVIterator::compareVertexPtr>::iterator it;
                unsigned int nb_neighbors = 0;
                for (it = adjVerticesSet.begin(); it != adjVerticesSet.end(); ++it)
                {
                    Vertex_ptr vi = *it;
                    p = vi->P();
                    pgi = GrenaillePoint::VectorType(p.x(), p.y(), p.z());

                    // Compute vertex normal
                    VFIterator vfIt = VFIterator(vi);
                    FaceList adjFaces = vfIt.list();
                    Vector3 n = Vector3(0.0, 0.0, 0.0);
                    for (unsigned int i = 0; i < adjFaces.size(); i++)
                    {
                        Vertex_ptr v0 = adjFaces[i]->HE()->V();
                        Vertex_ptr v1 = adjFaces[i]->HE()->Next()->V();
                        Vertex_ptr v2 = adjFaces[i]->HE()->Prev()->V();
                        n += Geometry::triangleNormal(v0->P(), v1->P(), v2->P());
                    }
                    n.normalize();
                    GrenaillePoint::VectorType ng = GrenaillePoint::VectorType(n.x(), n.y(), n.z());

                    GrenaillePoint gpi(pgi, ng);
                    if (fit.addNeighbor(gpi))
                    {
                        nb_neighbors++;
                    }
                }

                CORE_ASSERT(nb_neighbors >= 3, "PRIMITIVE COMPUTING DO NOT HAVE ENOUGH NEIGHBORS");

                fit.finalize();
                fit.applyPrattNorm();
                new_pg = fit.project(pg);
                error = (new_pg-pg).norm();
                pg = new_pg;
                nb_of_loop++;
                if (nb_of_loop > 16)
                    break;
            } while (error > 0.01); // TODO threshold

            if (fit.getCurrentState() != UNDEFINED)
            {
                q = fit;
                CORE_ASSERT(!std::isnan(q.m_uc), "PRIMITIVE NAN, NOT OK");
                //CORE_ASSERT(std::abs(q.m_uc) > 0.00000001 || std::abs(q.m_uq) > 0.00000001, "PRIMITIVE 0, NOT OK");
            }
            else
            {
                CORE_ASSERT(true, "PRIMITIVE NOT OK : APSS FIT IS NOT STABLE");
            }
        }

        void SimpleAPSSErrorMetric::generateFacePrimitive(Primitive &q, Face_ptr f, Dcel &dcel, Scalar weight, int ringSize)
        {
            Vertex_ptr v0 = f->HE()->V();
            Vertex_ptr v1 = f->HE()->Next()->V();
            Vertex_ptr v2 = f->HE()->Next()->Next()->V();
            Vector3 p = (v0->P() + v1->P() + v2->P()) / 3.0;
            GrenaillePoint::VectorType pg = GrenaillePoint::VectorType(p.x(), p.y(), p.z());
            GrenaillePoint::VectorType new_pg = GrenaillePoint::VectorType(p.x(), p.y(), p.z());

            Fit1 fit;
            fit.setWeightFunc(WeightFunc(weight)); // TODO weight func

            Scalar error;
            FFIterator ffIt = FFIterator(f);
            std::set<Face_ptr, FFIterator::compareFacePtr> adjFacesSet;
            ffIt.nRing(ringSize, adjFacesSet); // TODO N-ring
            //ffIt.nRing(1, adjFacesSet); // TODO N-ring
            do {
                fit.init(new_pg);
                GrenaillePoint::VectorType pgi;
                std::set<Face_ptr, FFIterator::compareFacePtr>::iterator it;
                for (it = adjFacesSet.begin(); it != adjFacesSet.end(); ++it)
                {
                    Face_ptr fi = *it;
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
                fit.applyPrattNorm();
                new_pg = fit.project(pg);
                error = (new_pg-pg).norm();
                pg = new_pg;
            } while (error > 0.01); // TODO threshold

            if (fit.getCurrentState() != UNDEFINED)
            {
                q = fit;
                CORE_ASSERT(!std::isnan(q.m_uc), "PRIMITIVE NAN, NOT OK");
                //CORE_ASSERT(std::abs(q.m_uc) > 0.00000001 || std::abs(q.m_uq) > 0.00000001, "PRIMITIVE 0, NOT OK");
            }
            else
            {
                CORE_ASSERT(true, "PRIMITIVE NOT OK : APSS FIT IS NOT STABLE");
            }
        }

    }
}


