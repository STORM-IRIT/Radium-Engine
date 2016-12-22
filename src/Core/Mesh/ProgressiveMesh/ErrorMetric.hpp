#ifndef ERROR_METRIC_H
#define ERROR_METRIC_H

#include <Core/Math/Quadric.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Patate/grenaille.h>

#include <iostream>
#include <fstream>

namespace Ra
{
    namespace Core
    {
        class QuadricErrorMetric
        {
            struct Param
            {
                Scalar scale;
            };

        public:
            using Primitive = Quadric<3>;

            QuadricErrorMetric();
            QuadricErrorMetric(Scalar scale);

            Scalar computeError(const Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult);
            Scalar computeGeometricError(const Primitive& q, const Primitive::Vector& p);

            void generateFacePrimitive(Primitive &q, const Face_ptr f, Dcel &dcel);

        private:
            Param m_param;
        };

        //---------------------------------------------------

        using namespace Grenaille;
        class GrenaillePoint
        {
        public:

            enum {Dim = 3};
            typedef typename ::Scalar Scalar;
            typedef Eigen::Matrix<Scalar, Dim, 1>   VectorType;

            inline GrenaillePoint(const VectorType &pos = VectorType::Zero(),
                                  const VectorType &normal = VectorType::Zero())
                : m_pos(pos), m_normal(normal)
            {}

            inline const VectorType& pos() const
            {
                return m_pos;
            }

            inline const VectorType& normal() const
            {
                return m_normal;
            }

            inline void setPos(VectorType &pos)
            {
                m_pos = pos;
            }

            inline void setNormal(VectorType &normal)
            {
                m_normal = normal;
            }


            inline void setPos(const Vector3 &v)
            {
                m_pos[0] = v.x();
                m_pos[1] = v.y();
                m_pos[2] = v.z();
            }

            inline void setNormal(const Vector3 &n)
            {
                m_normal[0] = n.x();
                m_normal[1] = n.y();
                m_normal[2] = n.z();
            }


        private:
            VectorType m_pos, m_normal;
        };

        class APSSErrorMetric
        {
            typedef DistWeightFunc<GrenaillePoint, ConstantWeightKernel<Scalar> > WeightFunc;
            typedef Basket<GrenaillePoint, WeightFunc, OrientedSphereFit, GLSParam> Fit1;
            typedef Basket<GrenaillePoint, WeightFunc, UnorientedSphereFit, GLSParam> Fit2;
            typedef Basket<GrenaillePoint, WeightFunc, OrientedSphereFit, GLSParam, OrientedSphereSpaceDer, GLSDer, GLSCurvatureHelper> Fit3;

            struct Param
            {
                Scalar scale;
                std::vector<Fit1> fits; //not needed except to see the spheres in debug mode
            };


        public:
            using Primitive = Quadric<4>;

            APSSErrorMetric();
            APSSErrorMetric(Scalar scale);

            Scalar computeError(const Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult);
            Scalar computeGeometricError(const Primitive& q, const Primitive::Vector& p);

            void generateFacePrimitive(Primitive &q, Face_ptr f, Dcel &dcel);

        //private:
        public:
            Param m_param;

        };

        /// On tente de venir projeter les point résultant sur
        /// la sphère moyenne
        class SimpleAPSSErrorMetric
        {
            //typedef DistWeightFunc<GrenaillePoint, ConstantWeightKernel<Scalar> > WeightFunc;
            typedef DistWeightFunc<GrenaillePoint, SmoothWeightKernel<Scalar> > WeightFunc;
            typedef Basket<GrenaillePoint, WeightFunc, OrientedSphereFit, GLSParam> Fit1;
            typedef Basket<GrenaillePoint, WeightFunc, UnorientedSphereFit, GLSParam> Fit2;
            typedef Basket<GrenaillePoint, WeightFunc, OrientedSphereFit, GLSParam, OrientedSphereSpaceDer, GLSDer, GLSCurvatureHelper> Fit3;

            struct Param
            {
                Scalar scale;
                std::vector<Fit1> fits; //not needed except to see the spheres in debug mode

            };


        public:
            using Primitive = Fit1;

            SimpleAPSSErrorMetric();
            SimpleAPSSErrorMetric(Scalar scale);

            Scalar computeError(Primitive& q, const Vector3& vs, const Vector3& vt, Vector3& pResult);
            Scalar computeGeometricError(const Primitive& q, const Vector3& p);
            Primitive combine(const Primitive& a, const Scalar& a_weight, const Primitive& b, const Scalar& b_weight, std::ofstream &file);
            Primitive combine(const Primitive& a, const Scalar& a_weight, const Primitive& b, const Scalar& b_weight);

            void generateFacePrimitive(Primitive &q, Face_ptr f, Dcel &dcel, Scalar mean_edge_size, Scalar scale);

        //private:
        public:
            Param m_param;

        };

    }




}

#include <Core/Mesh/ProgressiveMesh/ErrorMetric.inl>

#endif // ERROR_METRIC_H
