#include <Core/Animation/Skinning/LinearBlendSkinning.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void linearBlendSkinning( const Vector3Array& inMesh, const Pose& pose, const WeightMatrix& weight,
                          Vector3Array& outMesh ) {
    outMesh.clear();
    outMesh.resize( inMesh.size(), Vector3::Zero() );
    for ( int k = 0; k < weight.outerSize(); ++k )
    {
        const int nonZero = weight.col( k ).nonZeros();
        WeightMatrix::InnerIterator it0( weight, k );
#pragma omp parallel for
        for ( int nz = 0; nz < nonZero; ++nz )
        {
            WeightMatrix::InnerIterator it = it0 + Eigen::Index( nz );
            const uint i = it.row();
            const uint j = it.col();
            const Scalar w = it.value();
            outMesh[i] += w * ( pose[j] * inMesh[i] );
        }
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
