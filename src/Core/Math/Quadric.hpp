#ifndef QUADRIC_H
#define QUADRIC_H

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>


namespace Ra
{
    namespace Core
    {

        /// A representation for quadratic equation as
        /// v^T A v + 2 b^T v + c
        ///
        /// Reminder on ProgressiveMesh
        /// ---------------------------
        ///
        ///     Calculate the Quadric of the edge Qe = sum(Qf) with f all the faces adjacent
        ///     to e
        ///
        ///     Calculate vmin = Ae^(-1) be                with Qe = (Ae, be, ce)
        ///
        ///     Calculate the error on the edge
        ///         error = vmin^T Ae vmin + 2 be^T vmin + ce       with Qe = (Ae, be, ce)

        class Quadric
        {
        public:

            /// Construct an initialized quadric.
            Quadric();

            /// Construct and initialize a quadric.
            Quadric(Matrix3 a, Vector3 b, double c) : m_a(a), m_b(b), m_c(c) {}

            /// Construct a quadric from an other.
            Quadric(const Quadric &q);

            ~Quadric();

            /// Getters and setters

            inline const Matrix3& getA() const;
            inline void setA(const Matrix3 &a);

            inline const Vector3& getB() const;
            inline void setB(const Vector3 &b);

            inline const double & getC() const;
            inline void setC(const double &c);

            /// Create a quadric from a normal vector n and the scalar which
            /// represent the distance from the plane of normal n to the origin
            void compute(const Vector3& n, double ndotp);

            /// Operators

            inline Quadric operator+(const Quadric& q) const;
            inline Quadric operator*(const Scalar scal);

            inline Quadric& operator+=(const Quadric& q);
            inline Quadric& operator*=(Scalar scal);

        private:

            Matrix3 m_a;
            Vector3 m_b;
            double m_c;
        };

    }

}

#include <Core/Math/Quadric.inl>

#endif // QUADRIC_H
