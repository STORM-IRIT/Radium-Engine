#ifndef QUADRIC_H
#define QUADRIC_H

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>


namespace Ra
{
    namespace Core
    {

        class Quadric
        {
        public:

            /// Construct an initialized quadric.
            Quadric();

            /// Construct and initialize a quadric.
            Quadric(Matrix3d a, Vector3d b, double c) : m_a(a), m_b(b), m_c(c) {}

            /// Construct a quadric from an other.
            Quadric(const Quadric &q);

            ~Quadric();

            /// Getters and setters

            inline const Matrix3d& getA() const;
            inline void setA(const Matrix3d &a);

            inline const Vector3d& getB() const;
            inline void setB(const Vector3d &b);

            inline const double & getC() const;
            inline void setC(const double &c);

            /// Create a quadric from a normal vector n and the scalar which
            /// represent the distance from the plane of normal n to the origin
            void compute(const Vector3d& n, double ndotp);

            /// Operators

            inline Quadric operator+(const Quadric& q) const;
            inline Quadric operator*(const double scal);

            inline Quadric& operator+=(const Quadric& q);
            inline Quadric& operator*=(double scal);

        private:

            Matrix3d m_a;
            Vector3d m_b;
            double m_c;
        };

    }

}

#include <Core/Math/Quadric.inl>

#endif // QUADRIC_H
