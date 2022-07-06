#pragma once
#include <Core/Geometry/Curve2D.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

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

/*--------------------------------------------------*/

Curve2D::Vector PiecewiseCubicBezier::f( float u ) const {
    std::pair<int, float> locpar { getLocalParameter( u ) };

    if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
        Vector p;
        p.fill( 0 );
        return p;
    }

    return m_spline[locpar.first].f( locpar.second );
}

VectorArray<Curve2D::Vector> PiecewiseCubicBezier::f( std::vector<float> params ) const {
    VectorArray<Curve2D::Vector> controlPoints;

    for ( int i = 0; i < (int)params.size(); ++i ) {
        controlPoints.push_back( f( params[i] ) );
    }

    return controlPoints;
}

Curve2D::Vector PiecewiseCubicBezier::df( float u ) const {
    std::pair<int, float> locpar { getLocalParameter( u ) };

    if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
        Vector p;
        p.fill( 0 );
        return p;
    }

    return m_spline[locpar.first].df( locpar.second );
}

Curve2D::Vector PiecewiseCubicBezier::fdf( Scalar t, Vector& grad ) const {
    std::pair<int, float> locpar { getLocalParameter( t ) };

    if ( locpar.first < 0 || locpar.first > getNbBezier() - 1 ) {
        Vector p;
        p.fill( 0 );
        return p;
    }

    return m_spline[locpar.first].fdf( locpar.second, grad );
}

void PiecewiseCubicBezier::addPoint( const Curve2D::Vector p ) {
    if ( m_spline[m_spline.size() - 1].getCtrlPoints().size() < 4 )
        m_spline[m_spline.size() - 1].addPoint( p );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
