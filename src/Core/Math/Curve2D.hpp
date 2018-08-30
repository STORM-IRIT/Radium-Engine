#ifndef RADIUMENGINE_CURVE2D_HPP_
#define RADIUMENGINE_CURVE2D_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/Spline.hpp>
#include <Core/RaCore.hpp>
#include <vector>

namespace Ra {
namespace Core {

/// The Curve2D class is the base class to represent 2D parametric curves.
class Curve2D {
  public:
    /// The type of the curve.
    enum CurveType { LINE, CUBICBEZIER, SPLINE, SIZE };

    using Vector = Eigen::Matrix<Scalar, 2, 1>;

    /// Add a control point to the 2D curve.
    virtual void addPoint( const Vector p ) = 0;

    /// Return the point at the curvilinear parameter \p u.
    virtual Vector f( Scalar u ) const = 0;

    /// Return the tangent at the curvilinear parameter \p u.
    virtual Vector df( Scalar u ) const = 0;

    /// Return both the point and the tangent at the curvilinear parameter \p u.
    virtual Vector fdf( Scalar t, Vector& grad ) const = 0;

  protected:
    /// The number of control points.
    int size;
};

/// A Line represents a 2D segment.
class Line : public Curve2D {
  public:
    Line() {
        this->size = 0;
        m_points[0] = m_points[1] = Vector::Zero();
    }

    Line( const Vector& p0, const Vector& p1 ) : m_points{p0, p1} { this->size = 2; }

    /// Add a control point to the 2D curve.
    /// \note Adding a point beyond the second one does nothing.
    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    /// The segment endpoints.
    Vector m_points[2];
};

/// A QuadraSpline represents a 2D quadratic spline.
// FIXME: is this useful?
class QuadraSpline : public Curve2D {
  public:
    QuadraSpline() { this->size = 0; }

    QuadraSpline( const Curve2D::Vector& p0, const Curve2D::Vector& p1,
                  const Curve2D::Vector& p2 ) :
        m_points{p0, p1, p2} {
        this->size = 3;
    }

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar u, Vector& grad ) const override;

  private:
    /// The control points.
    Core::VectorArray<Vector> m_points;
};

/// A CubicBezier represents a 2D cubic Bezier curve.
class CubicBezier : public Curve2D {
  public:
    CubicBezier() {
        this->size = 0;
        m_points[0] = m_points[1] = m_points[2] = m_points[3] = Vector::Zero();
    }

    CubicBezier( const Curve2D::Vector& p0, const Curve2D::Vector& p1, const Curve2D::Vector& p2,
                 const Curve2D::Vector& p3 ) :
        m_points{p0, p1, p2, p3} {
        this->size = 4;
    }

    /// Add a control point to the 2D curve.
    /// \note Adding points beyond the fourth one does nothing.
    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    /// The 4 Bezier control points.
    Vector m_points[4];
};

/// A SplineCurve represents a 2D spline.
// FIXME: is this useful?
class SplineCurve : public Curve2D {
  public:
    SplineCurve() { this->size = 0; }

    SplineCurve( Core::VectorArray<Vector> points ) : m_points( points ) {
        this->size = points.size();
    }

    inline void addPoint( const Vector p ) override;

    inline Vector f( Scalar u ) const override;
    inline Vector df( Scalar u ) const override;
    inline Vector fdf( Scalar t, Vector& grad ) const override;

  private:
    /// The control points.
    Core::VectorArray<Vector> m_points;
};

} // namespace Core
} // namespace Ra

#include <Core/Math/Curve2D.inl>
#endif // RADIUMENGINE_CURVE2D_HPP_
