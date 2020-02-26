#include <Core/Animation/StretchableTwistableBoneSkinning.hpp>

#include <Core/Geometry/DistanceQueries.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void computeSTBS_weights( const Vector3Array& inMesh,
                          const Ra::Core::Animation::Skeleton& skel,
                          Ra::Core::Animation::WeightMatrix& weights ) {
    weights.resize( int( inMesh.size() ), skel.size() );
    std::vector<Eigen::Triplet<Scalar>> triplets;
    for ( int i = 0; i < weights.rows(); ++i )
    {
        const auto& pi = inMesh[uint( i )];
        for ( int j = 0; j < weights.cols(); ++j )
        {
            Ra::Core::Vector3 a, b;
            skel.getBonePoints( uint( j ), a, b );
            const Ra::Core::Vector3 ab = b - a;
            Scalar t                   = Ra::Core::Geometry::projectOnSegment( pi, a, ab );
            if ( t > 0 ) { triplets.push_back( Eigen::Triplet<Scalar>( i, j, t ) ); }
        }
    }
    weights.setFromTriplets( triplets.begin(), triplets.end() );
}

void linearBlendSkinningSTBS( const Vector3Array& inMesh,
                              const Pose& pose,
                              const Skeleton& poseSkel,
                              const Skeleton& restSkel,
                              const WeightMatrix& weightLBS,
                              const WeightMatrix& weightSTBS,
                              Vector3Array& outMesh ) {
    outMesh.clear();
    outMesh.resize( inMesh.size(), Vector3::Zero() );
    // first decompose all pose transforms
    Ra::Core::VectorArray<Matrix3> R( pose.size() );
    Matrix3 S; // we don't mind it
#pragma omp parallel for
    for ( int i = 0; i < int( pose.size() ); ++i )
    {
        pose[i].computeRotationScaling( &R[i], &S );
    }
    // then go through vertices
    for ( int k = 0; k < weightLBS.outerSize(); ++k )
    {
        const int nonZero = weightLBS.col( k ).nonZeros();
        WeightMatrix::InnerIterator it0( weightLBS, k );
#pragma omp parallel for
        for ( int nz = 0; nz < nonZero; ++nz )
        {
            WeightMatrix::InnerIterator it = it0 + Eigen::Index( nz );
            const uint i                   = it.row();
            const uint j                   = it.col();
            const Scalar w                 = it.value();
            Vector3 a, b, a_, b_;
            restSkel.getBonePoints( j, a, b );
            poseSkel.getBonePoints( j, a_, b_ );
            Vector3 si = ( std::sqrt( ( b_ - a_ ).squaredNorm() / ( b - a ).squaredNorm() ) - 1 ) *
                         ( b - a );
            outMesh[i] += w * ( a_ + R[j] * ( weightSTBS.coeff( i, j ) * si - a + inMesh[i] ) );
        }
    }
}

void computeDQSTBS( const Pose& pose,
                    const Skeleton& poseSkel,
                    const Skeleton& restSkel,
                    const WeightMatrix& weight,
                    const WeightMatrix& weightSTBS,
                    DQList& DQ ) {
    CORE_ASSERT( ( pose.size() == size_t( weight.cols() ) ), "pose/weight size mismatch." );
    DQ.clear();
    DQ.resize( weight.rows(),
               DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );
    // Stores the first non-zero quaternion for each vertex.
    std::vector<uint> firstNonZero( weight.rows(), std::numeric_limits<uint>::max() );
    // Contains the converted dual quaternions from the pose
    std::vector<DualQuaternion> poseDQ( pose.size() );
    // first decompose all pose transforms
    VectorArray<Matrix3> R( pose.size() );
    VectorArray<Vector3> A( pose.size() );
    VectorArray<Vector3> A_( pose.size() );
    VectorArray<Vector3> Si( pose.size() );
    Matrix3 S; // we don't mind it
#pragma omp parallel for
    for ( int i = 0; i < int( pose.size() ); ++i )
    {
        pose[i].computeRotationScaling( &R[i], &S );
        Ra::Core::Vector3 b, b_;
        restSkel.getBonePoints( i, A[i], b );
        poseSkel.getBonePoints( i, A_[i], b_ );
        Si[i] = ( std::sqrt( ( b_ - A_[i] ).squaredNorm() / ( b - A[i] ).squaredNorm() ) - 1 ) *
                ( b - A[i] );
    }
    // Loop through all transforms Tj
    for ( int k = 0; k < weight.outerSize(); ++k )
    {
        poseDQ[k] = DualQuaternion( pose[k] );
        // Count how many vertices are influenced by the given transform
        const int nonZero = weight.col( k ).nonZeros();
        WeightMatrix::InnerIterator it0( weight, k );
#pragma omp parallel for
        // This for loop is here just because OpenMP wants classic for loops.
        // Since we cannot iterate directly through the non-zero elements using the
        // InnerIterator, we initialize an InnerIterator to the first element and
        // then we increase it nz times.
        /*
         * This crappy piece of code was done in order to avoid the critical section
         *           DQ[i] += wq;
         *
         * that was occurring when parallelizing the main for loop.
         *
         * NOTE: this could be definitely improved by using std::thread
         */
        // Loop through all vertices vi who depend on Tj
        for ( int nz = 0; nz < nonZero; ++nz )
        {
            WeightMatrix::InnerIterator itn = it0 + Eigen::Index( nz );
            const uint i                    = itn.row();
            const uint j                    = itn.col();
            const Scalar w                  = itn.value();
            Transform T( R[j] );
            T.translation() = A_[j] + R[j] * ( weightSTBS.coeff( i, j ) * Si[j] - A[j] );
            auto D          = DualQuaternion( T );
            D.normalize();
            firstNonZero[i] = std::min( firstNonZero[i], uint( k ) );
            const Scalar sign =
                Ra::Core::Math::signNZ( poseDQ[j].getQ0().dot( poseDQ[firstNonZero[i]].getQ0() ) );
            const auto wq = D * w * sign;
            DQ[i] += wq;
        }
    }
    // Normalize all dual quats.
#pragma omp parallel for
    for ( int i = 0; i < int( DQ.size() ); ++i )
    {
        DQ[i].normalize();
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
