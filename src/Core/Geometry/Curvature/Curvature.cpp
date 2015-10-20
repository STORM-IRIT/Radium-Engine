#include <Core/Geometry/Curvature/Curvature.hpp>

#include <Core/Index/CircularIndex.hpp>

namespace Ra {
namespace Core {
namespace Geometry {


Scalar gaussianCurvature( const Vector3& v, const VectorArray< Vector3 >& p, const Scalar& area ) {
    CORE_ASSERT( ( area > 0.0 ), "The area has an invalid value" );
    Scalar theta = 0.0;
    uint N = p.size();
    CircularIndex i;
    i.setSize( N );
    for( uint j = 0; j < N; ++j ) {
        i.setValue( j );
        theta += Vector::angle( ( p[i] - v ), ( p[i-1] - v ) );
    }
    return ( ( Math::PiMul2 - theta ) / area );
}



Scalar gaussianCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 ) {
    return ( k1 * k2 );
}



Vector3 meanCurvatureNormal( const Vector3& laplacian, const Scalar& area ) {
    CORE_ASSERT( ( area > 0.0 ), "The area has an invalid value" );
    return ( laplacian / area );
}



Scalar meanCurvature( const Vector3& mean_curvature_normal ) {
    return ( 0.5 * mean_curvature_normal.norm() );
}



Scalar meanCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 ) {
    return ( 0.5 * ( k1 + k2 ) );
}



Scalar maxCurvature( const MeanCurvature& H, const GaussianCurvature& K ) {
    CORE_ASSERT( ( ( H * H ) >= K ), "Bad curvatures" );
    return ( H + std::sqrt( ( H * H ) - K ) );
}



Scalar minCurvature( const MeanCurvature& H, const GaussianCurvature& K ) {
    CORE_ASSERT( ( ( H * H ) >= K ), "Bad curvatures" );
    return ( H - std::sqrt( ( H * H ) - K ) );
}



}
}
}
