#ifndef ERROR_METRIC_H
#define ERROR_METRIC_H

#include <Core/Math/Quadric.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>


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
            QuadricErrorMetric(Param param);

            Scalar computeError(const Primitive& q, const Vector3& vs, const Vector3& vt,  Primitive::Vector& pResult);
            Scalar computeGeometricError(const Primitive& q, const Primitive::Vector& p);

            //void generateFacePrimitive(Primitive &q, Dcel dcel, Index fIndex);
            void generateFacePrimitive(Primitive &q, const Face_ptr f);

        private:
            Param m_param;
        };
    }

}

#include <Core/Mesh/ProgressiveMesh/ErrorMetric.inl>

#endif // ERROR_METRIC_H
