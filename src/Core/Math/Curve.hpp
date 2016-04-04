#ifndef CURVE
#define CURVE

#include <Core/RaCore.hpp>
#include <vector>
#include <Core/Containers/VectorArray.hpp>

namespace Ra
{
    namespace Core
    {

        enum CurveType
        {
            LINE,
            CUBICBEZIER,
            CurveTypeSize
        };

        template <uint D>
        class Curve
        {
        public:
            typedef typename Eigen::Matrix<Scalar, D, 1> Vector;

            virtual Vector f(Scalar u) const = 0;
            virtual Vector df( Scalar u ) const = 0;
            virtual Vector eval(Scalar t, Vector& grad) const = 0;

            //virtual void setPoint(const Vector& p, int idx) = 0;

        };

        class CubicBezier : public Curve<2>
        {
        public:
            CubicBezier(const Vector& p0, const Vector& p1, const Vector& p2, const Vector& p3) : m_points{p0, p1, p2, p3} {}
            inline Vector f(Scalar u) const override;
            inline Vector df( Scalar u ) const override;
            inline Vector eval(Scalar t, Vector& grad) const override;

            //inline void setPoint(const Vector& p, int idx) override;

        private:
            Vector m_points[4];
        };

        class Line : public Curve<2>
        {
        public:
            Line(const Vector& p0, const Vector& p1) : m_points{p0, p1} {}
            inline Vector f(Scalar u) const override;
            inline Vector df( Scalar u ) const override;
            inline Vector eval(Scalar t, Vector& grad) const override;

            //inline void setPoint(const Vector& p, int idx) override;

        private:
            Vector m_points[2];
        };

    }
}

#include <Core/Math/Curve.inl>
#endif // CURVE



