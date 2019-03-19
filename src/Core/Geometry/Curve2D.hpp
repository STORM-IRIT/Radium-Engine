#ifndef RADIUMENGINE_CURVE2D_HPP_
#define RADIUMENGINE_CURVE2D_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Spline.hpp>
#include <Core/RaCore.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * Generic class for 2D parametric curves.
 */
class Curve2D {
  public:
    /**
     * The type of curve.
     */
    enum CurveType {
        LINE,        ///< Segment.
        CUBICBEZIER, ///< Bézier cubic.
        SPLINE       ///< Spline.
    };

    /**
     * 2D point/vector type.
     */
    using Vector = Eigen::Matrix<Scalar, 2, 1>;

    virtual ~Curve2D() = default;

    /**
     * Register the given control point at the end.
     */
    virtual void addPoint( const Vector p ) = 0;

    /**
     * Return the point at parameter \p u.
     */
    virtual Vector f( Scalar u ) const = 0;

    /**
     * Return the tangent vector at parameter \p u.
     */
    virtual Vector df( Scalar u ) const = 0;

    /**
     * Return the point at parameter \p u, as well as the tangent vector in \p grad.
     */
    virtual Vector fdf( Scalar t, Vector& grad ) const = 0;

  protected:
    /// The number of control points.
    int size;
};

/**
 * The Line class represents 2D segments.
 */
class Line : public Curve2D {
  public:
    Line() { this->size = 0; }

    Line( const Vector& p0, const Vector& p1 ) : m_points{p0, p1} { this->size = 2; }

    ~Line() override = default;

    /**
     * Register the given control point at the end.
     * \note Does nothing if there are already 2 control points.
     */
    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;

    inline Vector df( Scalar u ) const override;

    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    /// The control points.
    Vector m_points[2];
};

/**
 * The CubicBezier class represents 2D cubic Bézier curves.
 */
class CubicBezier : public Curve2D {
  public:
    CubicBezier() { this->size = 0; }

    CubicBezier( const Curve2D::Vector& p0, const Curve2D::Vector& p1, const Curve2D::Vector& p2,
                 const Curve2D::Vector& p3 ) :
        m_points{p0, p1, p2, p3} {
        this->size = 4;
    }

    ~CubicBezier() override = default;

    /**
     * Register the given control point at the end.
     * \note Does nothing if there are already 4 control points.
     */
    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;

    inline Vector df( Scalar u ) const override;

    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    /// The control points.
    Vector m_points[4];
};

/**
 * The QuadraSpline class represents 2D quadratic splines.
 */
// FIXME: in the evaluation, Spline<2,2> is used, should be Spline<2,3>!
class QuadraSpline : public Curve2D {
  public:
    QuadraSpline() { this->size = 0; }

    QuadraSpline( const Curve2D::Vector& p0, const Curve2D::Vector& p1,
                  const Curve2D::Vector& p2 ) :
        m_points{p0, p1, p2} {
        this->size = 3;
    }

    ~QuadraSpline() override = default;

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;

    inline Vector df( Scalar u ) const override;

    inline Vector fdf( Scalar u, Vector& grad ) const override;

  private:
    /// The control points.
    Core::VectorArray<Vector> m_points;
};

/**
 * The SplineCurve class represents 2D splines.
 */
// FIXME: in the evaluation, Spline<2,3> is used, thus it's quadratic spline!
class SplineCurve : public Curve2D {
  public:
    SplineCurve() { this->size = 0; }
    SplineCurve( Core::VectorArray<Vector> points ) : m_points( points ) {
        this->size = points.size();
    }
    ~SplineCurve() override = default;

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;

    inline Vector df( Scalar u ) const override;

    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    /// The control points.
    Core::VectorArray<Vector> m_points;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Curve2D.inl>
#endif // RADIUMENGINE_CURVE2D_HPP_
