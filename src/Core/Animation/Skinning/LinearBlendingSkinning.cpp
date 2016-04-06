#include <Core/Animation/Skinning/LinearBlendingSkinning.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void linearBlendingSkinning( const Vector3Array&  inMesh,
                             const Pose&          pose,
                             const WeightMatrix&  weight,
                             Vector3Array&        outMesh ) {
    outMesh.resize( inMesh.size(), Vector3::Zero() );
    for( int k = 0; k < weight.outerSize(); ++k ) {
        for( WeightMatrix::InnerIterator it( weight, k ); it; ++it ) {
            const uint   i = it.row();
            const uint   j = it.col();
            const Scalar w = it.value();
            outMesh[i] += w * ( pose[j] * inMesh[i] );
        }
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra

