#include <Core/Math/Quadric.hpp>

namespace Ra
{
    namespace Core
    {

        Quadric::Quadric()
        {
            m_a = Matrix3::Zero();
            m_b = Vector3::Zero();
            m_c = 0.0;
        }


        Quadric::Quadric(const Quadric &q)
        {
            m_a = q.getA();
            m_b = q.getB();
            m_c = q.getC();
        }

        Quadric::~Quadric() {}

        void Quadric::compute(const Vector3& n, double ndotp)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    m_a(i, k) = n[i] * n[k];
                }
            }
            m_b = ndotp * n;
            m_c = ndotp * ndotp;
        }


    }
}
