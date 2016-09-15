#include "Quadric.hpp"

namespace Ra
{
    namespace Core
    {

        inline const Matrix3d& Quadric::getA() const
        {
            return m_a;
        }
        inline void Quadric::setA(const Eigen::Matrix3d &a)
        {
            m_a = a;
        }

        inline const Vector3d& Quadric::getB() const
        {
            return m_b;
        }
        inline void Quadric::setB(const Vector3d &b)
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
        inline Quadric Quadric::operator*(const double scal)
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
        inline Quadric& Quadric::operator*=(double scal)
        {
            m_a *= scal;
            m_b *= scal;
            m_c *= scal;
            return *this;
        }

    }
}
