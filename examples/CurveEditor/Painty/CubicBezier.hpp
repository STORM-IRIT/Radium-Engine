#pragma once

#include <Core/Types.hpp>
#include <Core/Utils/Log.hpp>
#include <algorithm>
#include <functional>
#include <numeric>

typedef Ra::Core::Vector2f ControlPoint;

/**
 * @brief Cubic Bézier segment
 */
template <typename T>
class CubicBezier
{
  public:
    using ControlPoint = T;

    /**
     * @brief Cubic Bezier segment, control polygon has 4 control points
     * @param control polygon (needs to be of size >= 4)
     * @param index of the first control point in the polygon (optional)
     */
    CubicBezier( const std::vector<ControlPoint>& cpoints, int _start = 0 ) {
        m_cpoints.insert(
            m_cpoints.begin(), cpoints.begin() + _start, cpoints.begin() + _start + 4 );
    }

    /**
     * @brief Computes a sample point in the bezier curve
     * @param t parameter of the sample, should be in [0,1]
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    ControlPoint pointAt( float t, int deriv = 0 ) const {
        std::vector<float> b = bernsteinCoefsAt( t, deriv );
        ControlPoint p;
        p.fill( 0. );
        return std::inner_product( b.begin(), b.end(), m_cpoints.cbegin(), p );
    }

    const std::vector<ControlPoint>& getCtrlPoints() const { return m_cpoints; }

    void setCtrlPoints( const std::vector<ControlPoint>& cpoints, int _start = 0 ) {
        m_cpoints.insert(
            m_cpoints.begin(), cpoints.begin() + _start, cpoints.begin() + _start + 4 );
    }

    /**
     * @brief Computes the cubic Bernstein coefficients for parameter t
     * @param t parameter of the coefficients
     * @param deriv derivative order
     * @return a vector of 4 scalar coefficients
     */
    static std::vector<float> bernsteinCoefsAt( float t, int deriv = 0 ) {
        if ( deriv == 2 )
            return { 6 * ( 1 - t ), 6 * ( -2 + 3 * t ), 6 * ( 1 - 3 * t ), 6 * t };
        else if ( deriv == 1 )
            return { -3 * powf( 1 - t, 2 ),
                     3 * ( 1 - t ) * ( 1 - 3 * t ),
                     3 * t * ( 2 - 3 * t ),
                     3 * powf( t, 2 ) };
        else
            return { powf( 1 - t, 3 ),
                     3 * t * powf( 1 - t, 2 ),
                     3 * powf( t, 2 ) * ( 1 - t ),
                     powf( t, 3 ) };
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

        ControlPoint p0 = pointAt( curParam );
        curParam += epsilon;

        while ( curParam <= end ) {
            ControlPoint p1 = pointAt( curParam );
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

  protected:
    std::vector<ControlPoint> m_cpoints; // Vector of control points of the Bezier segment
};

template <typename T>
class CubicBezierSpline
{
  public:
    using ControlPoint = T;
    using CubicBz      = CubicBezier<T>;

    CubicBezierSpline() {}

    /**
     * @brief Spline of cubic Bézier segments. Construction guarantees C0 continuity.
     *        ie extremities of successive segments share the same coordinates
     * @param vector of control points, should be 3*n+1 points where n is the number of segments
     */
    CubicBezierSpline( const std::vector<ControlPoint>& cpoints ) { setCtrlPoints( cpoints ); }

    CubicBezierSpline( const CubicBezierSpline& other ) { setCtrlPoints( other.getCtrlPoints() ); }

    int getNbBezier() const { return spline.size(); }

    const std::vector<CubicBz> getSplines() const { return spline; }

    /**
     * @brief Computes a sample point in the bezier spline
     * @param u global parameter of the sample, should be in [0,nbz]
     *        integer part of u represents the id of the Bézier segment
     *        while decimal part of u represents the local Bézier parameter
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    ControlPoint pointAt( float u, int deriv = 0 ) const {
        using namespace Ra::Core::Utils;
        std::pair<int, float> locpar { getLocalParameter( u ) };

        if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
            LOG( logERROR ) << "Cubic Bezier Spline : invalid parameter";
            ControlPoint p;
            p.fill( 0 );
            return p;
        }

        return spline[locpar.first].pointAt( locpar.second, deriv );
    }

    /**
     * @brief Computes a list of samples points in the bezier spline
     * @param list of u global parameter of the sample, should be in [0,nbz]
     *        integer part of u represents the id of the Bézier segment
     *        while decimal part of u represents the local Bézier parameter
     * @param deriv derivative order of the sampling
     * @return coordinates of the sample point
     */
    std::vector<ControlPoint> pointsAt( std::vector<float> params, int deriv = 0 ) const {
        std::vector<ControlPoint> controlPoints;

        for ( int i = 0; i < (int)params.size(); ++i ) {
            controlPoints.push_back( pointAt( params[i], deriv ) );
        }

        return controlPoints;
    }

    std::vector<ControlPoint> getCtrlPoints() const {
        std::vector<ControlPoint> cp;
        cp.reserve( 3 * getNbBezier() + 1 );
        for ( const CubicBz& bz : spline ) {
            cp.insert( cp.end(), bz.getCtrlPoints().begin(), bz.getCtrlPoints().end() - 1 );
        }
        if ( !spline.empty() ) { cp.emplace_back( spline.back().getCtrlPoints().back() ); }

        return cp;
    }

    void setCtrlPoints( const std::vector<ControlPoint>& cpoints ) {
        int nbz { (int)( ( cpoints.size() - 1 ) / 3 ) };
        spline.clear();
        spline.reserve( nbz );
        for ( int b = 0; b < nbz; ++b ) {
            spline.emplace_back( CubicBz( cpoints, 3 * b ) );
        }
    }

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
    std::vector<CubicBz> spline; // Vector of Bézier segments in the spline

    std::pair<int, float> getLocalParameter( float u ) const {
        return getLocalParameter( u, getNbBezier() );
    }
};

using CubicBezierSpline2f = CubicBezierSpline<Ra::Core::Vector2f>;
using CubicBezierSpline3f = CubicBezierSpline<Ra::Core::Vector3f>;
