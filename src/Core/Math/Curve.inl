#include <Core/Math/Curve.hpp>

namespace Ra
{
    namespace Core
    {
        Curve<2>::Vector CubicBezier::f(Scalar u) const
        {
            Vector grad;
            return eval(u, grad);
        }

        Curve<2>::Vector CubicBezier::df(Scalar u) const
        {
            Vector grad;
            eval(u, grad);
            return grad;
        }

        Curve<2>::Vector CubicBezier::eval(Scalar t, Vector& grad) const
        {
            float t2 = t * t;
            float t3 = t2 * t;
            float oneMinusT = 1.0 - t;
            float oneMinusT2 = oneMinusT * oneMinusT;
            float oneMinusT3 = oneMinusT2 * oneMinusT;

            grad = 3.0 * oneMinusT2 * (m_points[1] - m_points[0]) + 6.0 * oneMinusT * t * (m_points[2] - m_points[1]) + 3.0 * t2 * (m_points[3] - m_points[2]);
            return oneMinusT3 * m_points[0] + 3.0 * oneMinusT2 * t * m_points[1] + 3.0 * oneMinusT * t2 * m_points[2] + t3 * m_points[3];
        }

//        void CubicBezier::setPoint(const Curve::Vector &p, int idx)
//        {
//            if (idx > 0 && idx < 4)
//            {
//                m_points[idx] = p;
//            }
//        }

        Curve<2>::Vector Line::f(Scalar u) const
        {
            Vector grad;
            return eval(u, grad);
        }

        Curve<2>::Vector Line::df(Scalar u) const
        {
            Vector grad;
            eval(u, grad);
            return grad;
        }

        Curve<2>::Vector Line::eval(Scalar t, Vector& grad) const
        {
            grad = m_points[1] - m_points[0];
            return (1.0 - t) * m_points[0] + t * m_points[1];
        }

    }
}
