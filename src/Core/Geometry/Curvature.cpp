#include <Core/Geometry/Curvature.hpp>

#include <Core/Container/CircularIndex.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

// ========================================================
// ================== GAUSSIAN CURVATURE ==================
// ========================================================
Scalar gaussianCurvature( const Math::Vector3& v, const Container::VectorArray<Math::Vector3>& p, const Scalar& area ) {
    CORE_ASSERT( ( area > 0.0 ), "The area has an invalid value" );
    Scalar theta = 0.0;
    uint N = p.size();
    Container::CircularIndex i;
    i.setSize( N );
    for ( uint j = 0; j < N; ++j )
    {
        i.setValue( j );
        theta += Math::Vector::angle( ( p[i] - v ), ( p[i - 1] - v ) );
    }
    return ( ( Math::PiMul2 - theta ) / area );
}

void gaussianCurvature( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                        const AreaMatrix& A, Container::VectorArray<Scalar>& K ) {
    const uint size = p.size();
    K.clear();
    K.resize( size, 0.0 );
#pragma omp parallel for
    for ( int n = 0; n < int( T.size() ); ++n )
    {
        const Triangle& t = T[n];
        const uint i = t[0];
        const uint j = t[1];
        const uint k = t[2];

        const Math::Vector3 ij = ( p[j] - p[i] ).normalized();
        const Math::Vector3 jk = ( p[k] - p[j] ).normalized();
        const Math::Vector3 ki = ( p[i] - p[k] ).normalized();

#pragma omp critical
        {
            K[i] += Math::Vector::angle<Math::Vector3>( ij, -ki );
            K[j] += Math::Vector::angle<Math::Vector3>( jk, -ij );
            K[k] += Math::Vector::angle<Math::Vector3>( ki, -jk );
        }
    }

#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        K[i] = A.coeff( i, i ) * ( Math::PiMul2 - K[i] );
    }
}

Scalar gaussianCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 ) {
    return ( k1 * k2 );
}

void gaussianCurvature( const Container::VectorArray<MaximumCurvature>& k1,
                        const Container::VectorArray<MinimumCurvature>& k2, Container::VectorArray<Scalar>& K ) {
    const uint size = k1.size();
    K.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        K[i] = k1[i] * k2[i];
    }
}

// ========================================================
// ==================== MEAN CURVATURE ====================
// ========================================================
Math::Vector3 meanCurvatureNormal( const Math::Vector3& laplacian, const Scalar& area ) {
    CORE_ASSERT( ( area > 0.0 ), "The area has an invalid value" );
    return ( laplacian / area );
}

void meanCurvatureNormal( const Container::VectorArray<Math::Vector3>& laplacian, const AreaMatrix& A,
                          Container::VectorArray<Math::Vector3>& Hn ) {
    Hn.resize( laplacian.size() );
    Hn.getMap() = laplacian.getMap() * A.cwiseInverse();
}

Scalar meanCurvature( const Math::Vector3& mean_curvature_normal ) {
    return ( 0.5 * mean_curvature_normal.norm() );
}

void meanCurvature( const Container::VectorArray<Math::Vector3>& mean_curvature_normal, Container::VectorArray<Scalar>& H ) {
    const uint size = mean_curvature_normal.size();
    H.resize( size );
    H.getMap() = 0.5 * mean_curvature_normal.getMap().colwise().norm();
}

Scalar meanCurvature( const MaximumCurvature& k1, const MinimumCurvature& k2 ) {
    return ( 0.5 * ( k1 + k2 ) );
}

void meanCurvature( const Container::VectorArray<MaximumCurvature>& k1,
                    const Container::VectorArray<MinimumCurvature>& k2, Container::VectorArray<Scalar>& H ) {
    const uint size = k1.size();
    H.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        H[i] = 0.5 * ( k1[i] + k2[i] );
    }
}

// ========================================================
// ================== MAXIMUM  CURVATURE ==================
// ========================================================
Scalar maxCurvature( const MeanCurvature& H, const GaussianCurvature& K ) {
    if ( ( H * H ) < K )
    {
        return K;
    }
    // CORE_ASSERT( ( ( H * H ) >= K ), "Bad curvatures" );
    return ( H + std::sqrt( ( H * H ) - K ) );
}

void maxCurvature( const Container::VectorArray<MeanCurvature>& H, const Container::VectorArray<GaussianCurvature>& K,
                   Container::VectorArray<Scalar>& k1 ) {
    const uint size = H.size();
    k1.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
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

void minCurvature( const Container::VectorArray<MeanCurvature>& H, const Container::VectorArray<GaussianCurvature>& K,
                   Container::VectorArray<Scalar>& k2 ) {
    const uint size = H.size();
    k2.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        CORE_ASSERT( ( ( H[i] * H[i] ) >= K[i] ), "Bad curvatures" );
        k2[i] = H[i] - std::sqrt( ( H[i] * H[i] ) - K[i] );
    }
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
