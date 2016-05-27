#include <Core/Animation/Skinning/DualQuaternionSkinning.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void computeDQ( const Pose& pose, const WeightMatrix& weight, DQList& DQ ) {
    DQ.clear();
    DQ.resize( weight.rows(), DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );
    //#pragma omp parallel for
    for( int k = 0; k < weight.outerSize(); ++k ) {
        const DualQuaternion q( pose[k] );
        const int nonZero = weight.col( k ).nonZeros();
#if defined CORE_USE_OMP
        omp_set_dynamic(0);
        #pragma omp parallel for schedule( static ) num_threads(4)
#endif
        for( int nz = 0; nz < nonZero; ++nz ) {
            WeightMatrix::InnerIterator it( weight, k );
            for( int j = 0; j < nz; ++j ) {
                ++it;
            }
            const uint   i  = it.row();
            const Scalar w  = it.value();
            const auto   wq = q * w;
            //#pragma omp critical
            {
                DQ[i] += wq;
            }
        }
    }

    // Normalize all dual quats.
#if defined CORE_USE_OMP
    omp_set_dynamic(0);
    #pragma omp parallel for schedule( static ) num_threads(4)
#endif
    for( int i = 0; i < int(DQ.size()) ; ++i) {
        DQ[i].normalize();
    }
}

void DualQuaternionSkinning( const Vector3Array& input, const DQList& DQ, Vector3Array& output ) {
    const uint size = input.size();
    output.resize( size );
#if defined CORE_USE_OMP
    omp_set_dynamic(0);
    #pragma omp parallel for schedule( static ) num_threads(4)
#endif
    for( int i = 0; i < int(size); ++i ) {
        output[i] = DQ[i].transform( input[i] );
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
