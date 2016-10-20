#include "Quadric.hpp"

namespace Ra
{
    namespace Core
    {
        template<int DIM>
        inline Quadric<DIM>::Quadric() :
            m_a(Matrix::Zero()), m_b(Vector::Zero()), m_c(0) {}

        template<int DIM>
        inline Quadric<DIM>::Quadric(const Quadric<DIM> &q) :
            m_a(q.m_a), m_b(q.m_b), m_c(q.m_c) {}

        template<int DIM>
        inline Quadric<DIM>::Quadric(const Vector& n, double ndotp) :
            m_a(n * n.transpose()), m_b(ndotp * n), m_c(ndotp * ndotp) {}

        template<int DIM>
        inline Quadric<DIM>::~Quadric() {}

        template<int DIM>
        inline void Quadric<DIM>::compute(const Vector& n, double ndotp)
        {
            m_a = n * n.transpose();
            m_b = ndotp * n;
            m_c = ndotp * ndotp;
        }

        template<int DIM>
        inline const typename Quadric<DIM>::Matrix& Quadric<DIM>::getA() const
        {
            return m_a;
        }

        template<int DIM>
        inline void Quadric<DIM>::setA(const Matrix &a)
        {
            m_a = a;
        }

        template<int DIM>
        inline const typename Quadric<DIM>::Vector& Quadric<DIM>::getB() const
        {
            return m_b;
        }

        template<int DIM>
        inline void Quadric<DIM>::setB(const Vector &b)
        {
            m_b = b;
        }

        template<int DIM>
        inline const double & Quadric<DIM>::getC() const
        {
            return m_c;
        }

        template<int DIM>
        inline void Quadric<DIM>::setC(const double &c)
        {
            m_c = c;
        }

        template<int DIM>
        inline Quadric<DIM> Quadric<DIM>::operator+(const Quadric& q) const
        {
            return Quadric<DIM>(m_a + q.getA(), m_b + q.getB(), m_c + q.getC());
        }

        template<int DIM>
        inline Quadric<DIM> Quadric<DIM>::operator*(const Scalar scal)
        {
            return Quadric<DIM>(m_a * scal, m_b * scal, m_c * scal);
        }

        template<int DIM>
        inline Quadric<DIM>& Quadric<DIM>::operator+=(const Quadric& q)
        {
            m_a += q.getA();
            m_b += q.getB();
            m_c += q.getC();
            return *this;
        }

        template<int DIM>
        inline Quadric<DIM>& Quadric<DIM>::operator*=(Scalar scal)
        {
            m_a *= scal;
            m_b *= scal;
            m_c *= scal;
            return *this;
        }

    }
}
