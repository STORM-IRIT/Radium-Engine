#include "Quadric.hpp"

namespace Ra
{
    namespace Core
    {
        inline Quadric::Quadric() :
            m_a(Matrix3::Zero()), m_b(Vector3::Zero()), m_c(0) {}

        inline Quadric::Quadric(const Quadric &q) :
            m_a(q.m_a), m_b(q.m_b), m_c(q.m_c) {}

        inline Quadric::Quadric(const Vector3& n, double ndotp) :
            m_a(n * n.transpose()), m_b(ndotp * n), m_c(ndotp * ndotp) {}

        inline Quadric::~Quadric() {}

        inline void Quadric::compute(const Vector3& n, double ndotp)
        {
            m_a = n * n.transpose();
            m_b = ndotp * n;
            m_c = ndotp * ndotp;
        }

        inline const Matrix3& Quadric::getA() const
        {
            return m_a;
        }
        inline void Quadric::setA(const Matrix3 &a)
        {
            m_a = a;
        }

        inline const Vector3& Quadric::getB() const
        {
            return m_b;
        }
        inline void Quadric::setB(const Vector3 &b)
        {
            m_b = b;
        }

        inline const double & Quadric::getC() const
        {
            return m_c;
        }
        inline void Quadric::setC(const double &c)
        {
            m_c = c;
        }

        inline Quadric Quadric::operator+(const Quadric& q) const
        {
            return Quadric(m_a + q.getA(), m_b + q.getB(), m_c + q.getC());
        }
        inline Quadric Quadric::operator*(const Scalar scal)
        {
            return Quadric(m_a * scal, m_b * scal, m_c * scal);
        }

        inline Quadric& Quadric::operator+=(const Quadric& q)
        {
            m_a += q.getA();
            m_b += q.getB();
            m_c += q.getC();
            return *this;
        }
        inline Quadric& Quadric::operator*=(Scalar scal)
        {
            m_a *= scal;
            m_b *= scal;
            m_c *= scal;
            return *this;
        }

    }
}
