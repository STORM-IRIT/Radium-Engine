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

    /**
     * @brief Computes the cubic Bernstein coefficients for parameter t
     * @param t parameter of the coefficients
     * @param deriv derivative order
     * @return a vector of 4 scalar coefficients
     */
    static std::vector<float> bernsteinCoefsAt( float u, int deriv = 0 );

    /**
     * @brief get a list of curviline abscisses
     * @param distance in cm accross the curve that separate two params value
     * @param step sampling [0, 1]
     * @return list of params [0, 1]
     */
    std::vector<float> getArcLengthParameterization( float resolution, float epsilon ) const;

    const VectorArray<Vector> getCtrlPoints() const;

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

class CubicBezierSpline : public Curve2D
{
  public:
    CubicBezierSpline() {}

    /**
     * @brief Spline of cubic Bézier segments. Construction guarantees C0 continuity.
     *        ie extremities of successive segments share the same coordinates
     * @param vector of control points, should be 3*n+1 points where n is the number of segments
     */
    CubicBezierSpline( const Core::VectorArray<Vector>& cpoints ) { setCtrlPoints( cpoints ); }

    CubicBezierSpline( const CubicBezierSpline& other ) { setCtrlPoints( other.getCtrlPoints() ); }

    int getNbBezier() const { return m_spline.size(); }

    const std::vector<CubicBezier> getSplines() const { return m_spline; }

    /**
     * @brief Computes a sample point in the bezier spline
     * @param u global parameter of the sample, should be in [0,nbz]
     *        integer part of u represents the id of the Bézier segment
     *        while decimal part of u represents the local Bézier parameter
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    inline Vector f( float u ) const override;

    /**
     * @brief Computes a list of samples points in the bezier spline
     * @param list of u global parameter of the sample, should be in [0,nbz]
     *        integer part of u represents the id of the Bézier segment
     *        while decimal part of u represents the local Bézier parameter
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    inline VectorArray<Vector> f( std::vector<float> params ) const;

    inline Vector df( float u ) const override;

    inline Vector fdf( Scalar t, Vector& grad ) const override;

    inline void addPoint( const Vector p ) override;

    VectorArray<Vector> getCtrlPoints() const;

    void setCtrlPoints( const VectorArray<Vector>& cpoints );

    /**
     * @brief Decomposes a spline global parameter into the local Bézier parameters (static)
     * @param global parameter
     * @param number of segments in the spline
     * @return a pair (b,t) where b is the index of the bezier segment, and t the local parameter in
     * the segment
     */
    static std::pair<int, float> getLocalParameter( float u, int nbz );

    /**
     * @brief Map a normalized parameter for the spline to a global parameter
     * @param normalized parameter [0, 1]
     * @param number of segments in the spline
     * @return a global parameter t [0, nbz]
     */
    static float getGlobalParameter( float u, int nbz );

    /**
     * @brief equivalent to linspace function
     * @param number of param
     * @return a list of parameters t [0, nbz]
     */
    std::vector<float> getUniformParameterization( int nbSamples ) const;

    /**
     * @brief get a list of curviline abscisses
     * @param distance in cm accross the curve that separate two params value
     * @param step sampling [0, 1]
     * @return list of params [0, nbz]
     */
    std::vector<float> getArcLengthParameterization( float resolution, float epsilon ) const;

  private:
    std::vector<CubicBezier> m_spline; // Vector of Bézier segments in the spline

    std::pair<int, float> getLocalParameter( float u ) const {
        return getLocalParameter( u, getNbBezier() );
    }
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Curve2D.inl>
