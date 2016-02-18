#include <Core/Geometry/Curvature/Curvature.hpp>

#include <Core/Index/CircularIndex.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

// ========================================================
// ================== GAUSSIAN CURVATURE ==================
// ========================================================
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



void gaussianCurvature( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, const AreaMatrix& A, VectorArray< Scalar >& K ) {
    const uint size = p.size();
    K.clear();
    K.resize( size, 0.0 );
#pragma omp parallel for
    for( uint n = 0; n < T.size(); ++n ) {
        const Triangle& t = T[n];
        const uint i = t[0];
        const uint j = t[1];
        const uint k = t[2];

        const Vector3 ij = ( p[j] - p[i] ).normalized();
        const Vector3 jk = ( p[k] - p[j] ).normalized();
        const Vector3 ki = ( p[i] - p[k] ).normalized();

#pragma omp critical
        {
            K[i] += Vector::angle< Vector3 >( ij, -ki );
            K[j] += Vector::angle< Vector3 >( jk, -ij );
            K[k] += Vector::angle< Vector3 >( ki, -jk );
        }
    }

#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        K[i] = A.coeff( i, i ) * ( Math::PiMul2 - K[i] );
    }
}



Scalar gaussianCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 ) {
    return ( k1 * k2 );
}



void gaussianCurvature( const VectorArray< MaximumCurvature >& k1, const VectorArray< MinimumCurvature >& k2, VectorArray< Scalar >& K ) {
    const uint size = k1.size();
    K.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        K[i] = k1[i] * k2[i];
    }
}



// ========================================================
// ==================== MEAN CURVATURE ====================
// ========================================================
Vector3 meanCurvatureNormal( const Vector3& laplacian, const Scalar& area ) {
    CORE_ASSERT( ( area > 0.0 ), "The area has an invalid value" );
    return ( laplacian / area );
}



void meanCurvatureNormal( const VectorArray< Vector3 >& laplacian, const AreaMatrix& A, VectorArray< Vector3 >& Hn ) {
    Hn.resize( laplacian.size() );
    Hn.getMap() = laplacian.getMap() * A.cwiseInverse();
}



Scalar meanCurvature( const Vector3& mean_curvature_normal ) {
    return ( 0.5 * mean_curvature_normal.norm() );
}



void meanCurvature( const VectorArray< Vector3 >& mean_curvature_normal, VectorArray< Scalar >& H ) {
    const uint size = mean_curvature_normal.size();
    H.resize( size );
    H.getMap() = 0.5 * mean_curvature_normal.getMap().colwise().norm();
}



Scalar meanCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 ) {
    return ( 0.5 * ( k1 + k2 ) );
}



void meanCurvature( const VectorArray< MaximumCurvature >& k1, const VectorArray< MinimumCurvature >& k2, VectorArray< Scalar >& H ) {
    const uint size = k1.size();
    H.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        H[i] = 0.5 * ( k1[i] + k2[i] );
    }
}



// ========================================================
// ================== MAXIMUM  CURVATURE ==================
// ========================================================
Scalar maxCurvature( const MeanCurvature& H, const GaussianCurvature& K ) {
    if( ( H * H ) < K ) {
        return K;
    }
    //CORE_ASSERT( ( ( H * H ) >= K ), "Bad curvatures" );
    return ( H + std::sqrt( ( H * H ) - K ) );
}



void maxCurvature( const VectorArray< MeanCurvature >& H, const VectorArray< GaussianCurvature >& K, VectorArray< Scalar >& k1 ) {
    const uint size = H.size();
    k1.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        CORE_ASSERT( ( ( H[i] * H[i] ) >= K[i] ), "Bad curvatures" );
        k1[i] = H[i] + std::sqrt( ( H[i] * H[i] ) - K[i] );
    }
}



// ========================================================
// ================== MINIMUM  CURVATURE ==================
// ========================================================
Scalar minCurvature( const MeanCurvature& H, const GaussianCurvature& K ) {
    CORE_ASSERT( ( ( H * H ) >= K ), "Bad curvatures" );
    return ( H - std::sqrt( ( H * H ) - K ) );
}



void minCurvature( const VectorArray< MeanCurvature >& H, const VectorArray< GaussianCurvature >& K, VectorArray< Scalar >& k2 ) {
    const uint size = H.size();
    k2.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        CORE_ASSERT( ( ( H[i] * H[i] ) >= K[i] ), "Bad curvatures" );
        k2[i] = H[i] - std::sqrt( ( H[i] * H[i] ) - K[i] );
    }
}



}
}
}
