#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Spline.hpp>
#include <Core/RaCore.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {
class Curve2D
{
  public:
    enum CurveType { LINE, CUBICBEZIER, SPLINE, SIZE };

    using Vector = Eigen::Matrix<Scalar, 2, 1>;

    virtual void addPoint( const Vector p ) = 0;
    virtual ~Curve2D()                      = default;

    virtual Vector f( Scalar u ) const                 = 0;
    virtual Vector df( Scalar u ) const                = 0;
    virtual Vector fdf( Scalar t, Vector& grad ) const = 0;

  protected:
    int size;
};

class QuadraSpline : public Curve2D
{
  public:
    QuadraSpline() { this->size = 0; }
    QuadraSpline( const Curve2D::Vector& p0,
                  const Curve2D::Vector& p1,
                  const Curve2D::Vector& p2 ) :
        m_points { p0, p1, p2 } {
        this->size = 3;
    }
    ~QuadraSpline() override = default;

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar u, Vector& grad ) const override;

  private:
    Core::VectorArray<Vector> m_points;
};

class CubicBezier : public Curve2D
{
  public:
    CubicBezier() { this->size = 0; }
    CubicBezier( const Curve2D::Vector& p0,
                 const Curve2D::Vector& p1,
                 const Curve2D::Vector& p2,
                 const Curve2D::Vector& p3 ) :
        m_points { p0, p1, p2, p3 } {
        this->size = 4;
    }
    ~CubicBezier() override = default;

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    Vector m_points[4];
};

class Line : public Curve2D
{
  public:
    Line() { this->size = 0; }
    Line( const Vector& p0, const Vector& p1 ) : m_points { p0, p1 } { this->size = 2; }
    ~Line() override = default;

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    Vector m_points[2];
};

class SplineCurve : public Curve2D
{
  public:
    SplineCurve() { this->size = 0; }
    explicit SplineCurve( Core::VectorArray<Vector> points ) : m_points( points ) {
        this->size = points.size();
    }
    ~SplineCurve() override = default;

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    Core::VectorArray<Vector> m_points;
};

/*--------------------------------------------------*/

void CubicBezier::addPoint( const Curve2D::Vector p ) {
    if ( size < 4 ) { m_points[size++] = p; }
}

Curve2D::Vector CubicBezier::f( Scalar u ) const {
    Vector grad;
    return fdf( u, grad );
}

Curve2D::Vector CubicBezier::df( Scalar u ) const {
    Vector grad;
    fdf( u, grad );
    return grad;
}

Curve2D::Vector CubicBezier::fdf( Scalar t, Vector& grad ) const {
    float t2         = t * t;
    float t3         = t2 * t;
    float oneMinusT  = 1.0 - t;
    float oneMinusT2 = oneMinusT * oneMinusT;
    float oneMinusT3 = oneMinusT2 * oneMinusT;

    grad = 3.0 * oneMinusT2 * ( m_points[1] - m_points[0] ) +
           6.0 * oneMinusT * t * ( m_points[2] - m_points[1] ) +
           3.0 * t2 * ( m_points[3] - m_points[2] );
    return oneMinusT3 * m_points[0] + 3.0 * oneMinusT2 * t * m_points[1] +
           3.0 * oneMinusT * t2 * m_points[2] + t3 * m_points[3];
}

/*--------------------------------------------------*/

void Line::addPoint( const Curve2D::Vector p ) {
    if ( size < 2 ) { m_points[size++] = p; }
}

Curve2D::Vector Line::f( Scalar u ) const {
    return ( 1.0 - u ) * m_points[0] + u * m_points[1];
}

Curve2D::Vector Line::df( Scalar /*u*/ ) const {
    return m_points[1] - m_points[0];
}

Curve2D::Vector Line::fdf( Scalar t, Vector& grad ) const {
    grad = m_points[1] - m_points[0];
    return ( 1.0 - t ) * m_points[0] + t * m_points[1];
}

/*--------------------------------------------------*/

void SplineCurve::addPoint( const Curve2D::Vector p ) {
    m_points.push_back( p );
    ++size;
}

Curve2D::Vector SplineCurve::f( Scalar u ) const {
    Spline<2, 3> spline;
    spline.setCtrlPoints( m_points );

    return spline.f( u );
}

Curve2D::Vector SplineCurve::df( Scalar u ) const {
    Spline<2, 3> spline;
    spline.setCtrlPoints( m_points );

    return spline.df( u );
}

Curve2D::Vector SplineCurve::fdf( Scalar u, Curve2D::Vector& grad ) const {
    Spline<2, 3> spline;
    spline.setCtrlPoints( m_points );

    grad = spline.df( u );
    return spline.f( u );
}

/*--------------------------------------------------*/

void QuadraSpline::addPoint( const Curve2D::Vector p ) {
    m_points.push_back( p );
    ++size;
}

Curve2D::Vector QuadraSpline::f( Scalar u ) const {
    Spline<2, 2> spline;
    spline.setCtrlPoints( m_points );

    return spline.f( u );
}

Curve2D::Vector QuadraSpline::df( Scalar u ) const {
    Spline<2, 2> spline;
    spline.setCtrlPoints( m_points );

    return spline.df( u );
}

Curve2D::Vector QuadraSpline::fdf( Scalar u, Vector& grad ) const {
    Spline<2, 2> spline;
    spline.setCtrlPoints( m_points );

    grad = spline.df( u );
    return spline.f( u );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
