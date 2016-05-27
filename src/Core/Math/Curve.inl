#include <Core/Math/Curve.hpp>

namespace Ra
{
    namespace Core
    {


        /*--------------------------------------------------*/


        void CubicBezier::addPoint(const Curve::Vector p)
        {
            if(size < 4)
            {
                m_points[size ++] = p;
            }
        }

        Curve::Vector CubicBezier::f(Scalar u) const
        {
            Vector grad;
            return fdf(u, grad);
        }

        Curve::Vector CubicBezier::df(Scalar u) const
        {
            Vector grad;
            fdf(u, grad);
            return grad;
        }

        Curve::Vector CubicBezier::fdf(Scalar t, Vector& grad) const
        {
            float t2 = t * t;
            float t3 = t2 * t;
            float oneMinusT = 1.0 - t;
            float oneMinusT2 = oneMinusT * oneMinusT;
            float oneMinusT3 = oneMinusT2 * oneMinusT;

            grad = 3.0 * oneMinusT2 * (m_points[1] - m_points[0]) + 6.0 * oneMinusT * t * (m_points[2] - m_points[1]) + 3.0 * t2 * (m_points[3] - m_points[2]);
            return oneMinusT3 * m_points[0] + 3.0 * oneMinusT2 * t * m_points[1] + 3.0 * oneMinusT * t2 * m_points[2] + t3 * m_points[3];
        }

        /*--------------------------------------------------*/

        void Line::addPoint(const Curve::Vector p)
        {
            if(size < 2)
            {
                m_points[size ++] = p;
            }
        }

        Curve::Vector Line::f(Scalar u) const
        {
            return (1.0 - u) * m_points[0] + u * m_points[1];
        }

        Curve::Vector Line::df(Scalar u) const
        {
            return m_points[1] - m_points[0];
        }

        Curve::Vector Line::fdf(Scalar t, Vector& grad) const
        {
            grad = m_points[1] - m_points[0];
            return (1.0 - t) * m_points[0] + t * m_points[1];
        }

        /*--------------------------------------------------*/


        void SplineCurve::addPoint(const Curve::Vector p)
        {
            m_points.push_back(p);
            ++size;
        }

        Curve::Vector SplineCurve::f(Scalar u) const
        {
           Ra::Core::Spline<2, 3> spline;
           spline.setCtrlPoints(m_points);

           return spline.f(u);
        }

        Curve::Vector SplineCurve::df(Scalar u) const
        {
            Ra::Core::Spline<2, 3> spline;
            spline.setCtrlPoints(m_points);

            return spline.df(u);
        }

        Curve::Vector SplineCurve::fdf(Scalar u, Curve::Vector &grad) const
        {
            Ra::Core::Spline<2, 3> spline;
            spline.setCtrlPoints(m_points);

            grad = spline.df(u);
            return spline.f(u);
        }

        /*--------------------------------------------------*/


        void QuadraSpline::addPoint(const Curve::Vector p)
        {
            m_points.push_back(p);
            ++size;
        }

        Curve::Vector QuadraSpline::f(Scalar u) const
        {
            Ra::Core::Spline<2, 2> spline;
            spline.setCtrlPoints(m_points);

            return spline.f(u);
        }

        Curve::Vector QuadraSpline::df(Scalar u) const
        {
            Ra::Core::Spline<2, 2> spline;
            spline.setCtrlPoints(m_points);

            return spline.df(u);
        }

        Curve::Vector QuadraSpline::fdf(Scalar u, Vector& grad) const
        {
            Ra::Core::Spline<2, 2> spline;
            spline.setCtrlPoints(m_points);

            grad = spline.df(u);
            return spline.f(u);
        }



    }
}
