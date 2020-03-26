#include <Core/Animation/DualQuaternionSkinning.hpp>

namespace Ra {
namespace Core {
namespace Animation {
void computeDQ( const Pose& pose, const Sparse& weight, DQList& DQ ) {
    CORE_ASSERT( ( pose.size() == size_t( weight.cols() ) ), "pose/weight size mismatch." );
    DQ.clear();
    DQ.resize( weight.rows(),
               DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );

    // Stores the first non-zero quaternion for each vertex.
    std::vector<uint> firstNonZero( weight.rows(), std::numeric_limits<uint>::max() );
    // Contains the converted dual quaternions from the pose
    std::vector<DualQuaternion> poseDQ( pose.size() );

    // Loop through all transforms Tj
    for ( int j = 0; j < weight.outerSize(); ++j )
    {
        poseDQ[j] = DualQuaternion( pose[j] );
        // Count how many vertices are influenced by the given transform
        const int nonZero = weight.col( j ).nonZeros();

        Sparse::InnerIterator it0( weight, j );
#pragma omp parallel for
        // This for loop is here just because OpenMP wants classic for loops.
        // Since we cannot iterate directly through the non-zero elements using the InnerIterator,
        // we initialize an InnerIterator to the first element and then we increase it nz times.
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
            Sparse::InnerIterator itn = it0 + Eigen::Index( nz );
            const uint i              = itn.row();
            const Scalar w            = itn.value();

            firstNonZero[i] = std::min( firstNonZero[i], uint( j ) );
            const Scalar sign =
                Ra::Core::Math::signNZ( poseDQ[j].getQ0().dot( poseDQ[firstNonZero[i]].getQ0() ) );

            const auto wq = poseDQ[j] * w * sign;
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

// alternate naive version, for reference purposes.
// See Kavan , Collins, Zara and O'Sullivan, 2008
void computeDQ_naive( const Pose& pose, const Sparse& weight, DQList& DQ ) {
    CORE_ASSERT( ( pose.size() == size_t( weight.cols() ) ), "pose/weight size mismatch." );
    DQ.clear();
    DQ.resize( weight.rows(),
               DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );
    std::vector<DualQuaternion> poseDQ( pose.size() );

    // 1. Convert all transforms to DQ
#pragma omp parallel for
    for ( int j = 0; j < weight.cols(); ++j )
    {
        poseDQ[j] = DualQuaternion( pose[j] );
    }

    // 2. for all vertices, blend the dual quats.
    for ( int i = 0; i < weight.rows(); ++i )
    {
        int firstNonZero = -1;
        for ( uint j = 0; j < weight.cols(); ++j )
        {
            const Scalar& w = weight.coeff( i, j );
            if ( w == 0 ) { continue; }

            if ( firstNonZero < 0 ) { firstNonZero = j; }

            // Flip the dual quaternion sign according to the dot product with the first non-null
            // dual quat see Algorithm 2 in section 4.1 of the paper.
            Scalar sign =
                Ra::Core::Math::signNZ( poseDQ[j].getQ0().dot( poseDQ[firstNonZero].getQ0() ) );

            DQ[i] += sign * w * poseDQ[j];
        }
    }

    // 3. renormalize all dual quats.
#pragma omp parallel for
    for ( int i = 0; i < int( DQ.size() ); ++i )
    {
        DQ[i].normalize();
    }
}

void dualQuaternionSkinning( const Ra::Core::Vector3Array& input,
                             const DQList& DQ,
                             Ra::Core::Vector3Array& output ) {
    const uint size = input.size();
    CORE_ASSERT( ( size == DQ.size() ), "input/DQ size mismatch." );
    output.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        output[i] = DQ[i].transform( input[i] );
    }
}
} // namespace Animation
} // namespace Core
} // namespace Ra
