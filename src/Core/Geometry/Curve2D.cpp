#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Curve2D.hpp>

using namespace Ra::Core;
using namespace Ra::Core::Geometry;

std::pair<int, float> CubicBezierSpline::getLocalParameter( float u, int nbz ) {
    int b { (int)( std::floor( u ) ) };
    float t { u - b };

    if ( ( b == nbz ) && ( t == 0 ) ) {
        b = nbz - 1;
        t = 1;
    }
    return { b, t };
}

float CubicBezierSpline::getGlobalParameter( float u, int nbz ) {
    return u * nbz;
}

VectorArray<Curve2D::Vector> CubicBezierSpline::getCtrlPoints() const {
    VectorArray<Curve2D::Vector> cp;
    cp.reserve( 3 * getNbBezier() + 1 );
    for ( unsigned int i = 0; i < spline.size(); i++ ) {
        cp.push_back( spline[i].getCtrlPoints()[0] );
        cp.push_back( spline[i].getCtrlPoints()[1] );
        cp.push_back( spline[i].getCtrlPoints()[2] );
    }
    if ( !spline.empty() ) { cp.push_back( spline[spline.size() - 1].getCtrlPoints()[3] ); }

    return cp;
}

void CubicBezierSpline::setCtrlPoints( const VectorArray<Curve2D::Vector>& cpoints ) {
    int nbz { (int)( ( cpoints.size() - 1 ) / 3 ) };
    spline.clear();
    spline.reserve( nbz );
    for ( int b = 0; b < nbz; ++b ) {
        spline.emplace_back( CubicBezier(
            cpoints[3 * b], cpoints[3 * b + 1], cpoints[3 * b + 2], cpoints[3 * b + 3] ) );
    }
}

std::vector<float> CubicBezierSpline::getUniformParameterization( int nbSamples ) const {
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

std::vector<float> CubicBezierSpline::getArcLengthParameterization( float resolution,
                                                                    float epsilon ) const {

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

std::pair<int, float> CubicBezierSpline::getLocalParameter( float u ) const {
    return getLocalParameter( u, getNbBezier() );
}
