#ifndef CURVE
#define CURVE

#include <Core/RaCore.hpp>
#include <vector>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/Spline.hpp>

namespace Ra
{
    namespace Core
    {



        class Curve
        {
        public:
            enum CurveType
            {
                LINE,
                CUBICBEZIER,
                SPLINE,
                SIZE
            };

            typedef typename Eigen::Matrix<Scalar, 2, 1> Vector;

            virtual void addPoint(const Vector p) = 0;

            virtual Vector f(Scalar u) const = 0;
            virtual Vector df( Scalar u ) const = 0;
            virtual Vector fdf(Scalar t, Vector& grad) const = 0;

        protected:
            int size;
        };

        class CubicBezier : public Curve
        {
        public:
            CubicBezier() { this->size = 0; }
            CubicBezier (const Curve::Vector &p0, const Curve::Vector &p1,
                                      const Curve::Vector &p2, const Curve::Vector &p3)
                : m_points{p0, p1, p2, p3} { this->size = 4; }


            inline  void addPoint(const Vector p) override;

            inline Vector f(Scalar u) const override;
            inline Vector df( Scalar u ) const override;
            inline Vector fdf(Scalar t, Vector& grad) const override;

        private:
            Vector m_points[4];
        };

        class Line : public Curve
        {
        public:
            Line () { this->size = 0; }
            Line(const Vector& p0, const Vector& p1) : m_points{p0, p1} { this->size = 2; }

            inline  void addPoint(const Vector p) override;

            inline Vector f(Scalar u) const override;
            inline Vector df( Scalar u ) const override;
            inline Vector fdf(Scalar t, Vector& grad) const override;

        private:
            Vector m_points[2];
        };

        class SplineCurve : public Curve
        {
        public:
            SplineCurve() { this->size = 0; }
            SplineCurve(Core::VectorArray<Vector> points):m_points(points) { this->size = points.size();}

            inline  void addPoint(const Vector p) override;

            inline Vector f(Scalar u) const override;
            inline Vector df( Scalar u ) const override;
            inline Vector fdf(Scalar t, Vector& grad) const override;

        private:
            Core::VectorArray<Vector> m_points;

        };

    }
}

#include <Core/Math/Curve.inl>
#endif // CURVE



