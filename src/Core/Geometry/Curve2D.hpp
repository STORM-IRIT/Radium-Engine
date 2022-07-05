#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Spline.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Log.hpp>
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
    static std::vector<float> bernsteinCoefsAt( float u, int deriv = 0 ) {
        if ( deriv == 2 ) {
            return { 6 * ( 1 - u ), 6 * ( -2 + 3 * u ), 6 * ( 1 - 3 * u ), 6 * u };
        }
        else if ( deriv == 1 )
            return { -3 * powf( 1 - u, 2 ),
                     3 * ( 1 - u ) * ( 1 - 3 * u ),
                     3 * u * ( 2 - 3 * u ),
                     3 * powf( u, 2 ) };
        else
            return { powf( 1 - u, 3 ),
                     3 * u * powf( 1 - u, 2 ),
                     3 * powf( u, 2 ) * ( 1 - u ),
                     powf( u, 3 ) };
    }

    /**
     * @brief get a list of curviline abscisses
     * @param distance in cm accross the curve that separate two params value
     * @param step sampling [0, 1]
     * @return list of params [0, 1]
     */
    std::vector<float> getArcLengthParameterization( float resolution, float epsilon ) const {
        std::vector<float> params;
        float start    = 0.0f;
        float end      = 1.0f;
        float curParam = start;
        float curDist  = 0.0f;

        params.push_back( curParam );

        Vector p0 = f( curParam );
        curParam += epsilon;

        while ( curParam <= end ) {
            Vector p1 = f( curParam );
            curDist += sqrt( pow( p0.x() - p1.x(), 2 ) + pow( p0.y() - p1.y(), 2 ) );
            if ( curDist >= resolution ) {
                params.push_back( curParam );
                curDist = 0.0f;
            }
            p0 = p1;
            curParam += epsilon;
        }

        // push last sample point to the end to ensure bounds [0, 1]
        params[params.size() - 1] = end;

        return params;
    }

    const VectorArray<Vector> getCtrlPoints() const {
        VectorArray<Vector> ctrlPts;
        ctrlPts.reserve( 4 );
        ctrlPts.push_back( m_points[0] );
        ctrlPts.push_back( m_points[1] );
        ctrlPts.push_back( m_points[2] );
        ctrlPts.push_back( m_points[3] );
        return ctrlPts;
    }

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

    int getNbBezier() const { return spline.size(); }

    const std::vector<CubicBezier> getSplines() const { return spline; }

    /**
     * @brief Computes a sample point in the bezier spline
     * @param u global parameter of the sample, should be in [0,nbz]
     *        integer part of u represents the id of the Bézier segment
     *        while decimal part of u represents the local Bézier parameter
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    inline Vector f( float u ) const override {
        using namespace Ra::Core::Utils;
        std::pair<int, float> locpar { getLocalParameter( u ) };

        if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
            LOG( logERROR ) << "Cubic Bezier Spline : invalid parameter";
            Vector p;
            p.fill( 0 );
            return p;
        }

        return spline[locpar.first].f( locpar.second );
    }

    inline Vector df( float u ) const override {
        using namespace Ra::Core::Utils;
        std::pair<int, float> locpar { getLocalParameter( u ) };

        if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
            LOG( logERROR ) << "Cubic Bezier Spline : invalid parameter";
            Vector p;
            p.fill( 0 );
            return p;
        }

        return spline[locpar.first].df( locpar.second );
    }

    inline Vector fdf( Scalar t, Vector& grad ) const override {
        using namespace Ra::Core::Utils;
        std::pair<int, float> locpar { getLocalParameter( t ) };

        if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
            LOG( logERROR ) << "Cubic Bezier Spline : invalid parameter";
            Vector p;
            p.fill( 0 );
            return p;
        }

        return spline[locpar.first].fdf( locpar.second, grad );
    }

    /**
     * @brief Computes a list of samples points in the bezier spline
     * @param list of u global parameter of the sample, should be in [0,nbz]
     *        integer part of u represents the id of the Bézier segment
     *        while decimal part of u represents the local Bézier parameter
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    VectorArray<Vector> f( std::vector<float> params ) const {
        VectorArray<Vector> controlPoints;

        for ( int i = 0; i < (int)params.size(); ++i ) {
            controlPoints.push_back( f( params[i] ) );
        }

        return controlPoints;
    }

    VectorArray<Vector> getCtrlPoints() const {
        VectorArray<Curve2D::Vector> cp;
        std::cout << "nb bezier: " << getNbBezier() << std::endl;
        cp.reserve( 3 * getNbBezier() + 1 );
        std::cout << "spline size: " << spline.size() << std::endl;
        std::cout << cp.size() << std::endl;
        for ( unsigned int i = 0; i < spline.size(); i++ ) {
            cp.push_back( spline[i].getCtrlPoints()[0] );
            cp.push_back( spline[i].getCtrlPoints()[1] );
            cp.push_back( spline[i].getCtrlPoints()[2] );
        }
        if ( !spline.empty() ) { cp.push_back( spline[spline.size() - 1].getCtrlPoints()[3] ); }
        std::cout << "cp size: " << cp.size() << std::endl;

        return cp;
    }

    void setCtrlPoints( const VectorArray<Vector>& cpoints ) {
        int nbz { (int)( ( cpoints.size() - 1 ) / 3 ) };
        spline.clear();
        spline.reserve( nbz );
        for ( int b = 0; b < nbz; ++b ) {
            spline.emplace_back( CubicBezier(
                cpoints[3 * b], cpoints[3 * b + 1], cpoints[3 * b + 2], cpoints[3 * b + 3] ) );
        }
    }

    inline void addPoint( const Vector p ) override {

    };

    /**
     * @brief Decomposes a spline global parameter into the local Bézier parameters (static)
     * @param global parameter
     * @param number of segments in the spline
     * @return a pair (b,t) where b is the index of the bezier segment, and t the local parameter in
     * the segment
     */
    static std::pair<int, float> getLocalParameter( float u, int nbz ) {
        int b { (int)( std::floor( u ) ) };
        float t { u - b };

        if ( ( b == nbz ) && ( t == 0 ) ) {
            b = nbz - 1;
            t = 1;
        }
        return { b, t };
    }

    /**
     * @brief Map a normalized parameter for the spline to a global parameter
     * @param normalized parameter [0, 1]
     * @param number of segments in the spline
     * @return a global parameter t [0, nbz]
     */
    static float getGlobalParameter( float u, int nbz ) { return u * nbz; }

    /**
     * @brief equivalent to linspace function
     * @param number of param
     * @return a list of parameters t [0, nbz]
     */
    std::vector<float> getUniformParameterization( int nbSamples ) const {
        std::vector<float> params;
        params.resize( nbSamples );

        float delta = { 1.0f / (float)( nbSamples - 1 ) };
        float acc   = 0.0f;
        int nbz     = getNbBezier();

        params[0] = getGlobalParameter( 0.0f, nbz );
        for ( int i = 1; i < nbSamples; ++i ) {
            acc += delta;
            params[i] = getGlobalParameter( acc, nbz );
        }

        params[params.size() - 1] = getGlobalParameter( 1.0f, nbz );

        return params;
    }

    /**
     * @brief get a list of curviline abscisses
     * @param distance in cm accross the curve that separate two params value
     * @param step sampling [0, 1]
     * @return list of params [0, nbz]
     */
    std::vector<float> getArcLengthParameterization( float resolution, float epsilon ) const {

        std::vector<float> params;
        int nbz = getNbBezier();

        if ( nbz <= 0 ) return params;

        params = spline[0].getArcLengthParameterization( resolution, epsilon );

        for ( int i = 1; i < nbz; ++i ) {
            std::vector<float> tmpParams =
                spline[i].getArcLengthParameterization( resolution, epsilon );
            std::transform( tmpParams.begin(),
                            tmpParams.end(),
                            tmpParams.begin(),
                            [&]( auto const& elem ) { return elem + i; } );
            params.insert( params.end(), tmpParams.begin() + 1, tmpParams.end() );
        }

        return params;
    }

  private:
    std::vector<CubicBezier> spline; // Vector of Bézier segments in the spline

    std::pair<int, float> getLocalParameter( float u ) const {
        return getLocalParameter( u, getNbBezier() );
    }
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
