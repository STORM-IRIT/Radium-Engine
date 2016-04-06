#include <Core/Animation/Skinning/DualQuaternionSkinning.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void computeDQ( const Pose& pose, const WeightMatrix& weight, DQList& DQ ) {
    DQ.clear();
    DQ.resize( weight.rows(), DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );
    #pragma omp parallel for
        for( uint k = 0; k < weight.outerSize(); ++k ) {
            DualQuaternion q( pose[k] );
            for( WeightMatrix::InnerIterator it( weight, k ); it; ++it ) {
                const uint   i = it.row();
                const Scalar w = it.value();
                const auto wq = q * w;
    #pragma omp critical
                {
                    DQ[i] += wq;
                }
            }
        }

        // Normalize all dual quats.
    #pragma omp parallel for
        for( uint i = 0; i < DQ.size() ; ++i) {
            DQ[i].normalize();
        }
}

void DualQuaternionSkinning( const Vector3Array& input, const DQList& DQ, Vector3Array& output ) {
    const uint size = input.size();
    output.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        output[i] = DQ[i].transform( input[i] );
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
