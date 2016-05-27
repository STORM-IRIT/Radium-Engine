#ifndef RADIUM_ENGINE_DUAL_QUATERNION_SKINNING_HPP
#define RADIUM_ENGINE_DUAL_QUATERNION_SKINNING_HPP

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace SkinningPlugin {

using Ra::Core::VectorN;
using Ra::Core::Quaternion;
using Ra::Core::DualQuaternion;
typedef Ra::Core::AlignedStdVector< DualQuaternion > DQList;
typedef Ra::Core::Vector3Array VertexList;
using Ra::Core::Animation::Pose;
using Ra::Core::Animation::WeightMatrix;


inline void computeDQ( const Pose& pose, const WeightMatrix& weight, DQList& DQ ) {
    DQ.clear();
    DQ.resize( weight.rows(), DualQuaternion( Quaternion(0,0,0,0), Quaternion(0,0,0,0) ) );
    #pragma omp parallel for
        for( int k = 0; k < weight.outerSize(); ++k ) {
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
        for(int i = 0; i < int(DQ.size()) ; ++i) {
            DQ[i].normalize();
        }
}

inline void DualQuaternionSkinning( const VertexList& input, const DQList& DQ, VertexList& output ) {
    const uint size = input.size();
    output.resize( size );
#pragma omp parallel for
    for( int i = 0; i < int(size); ++i ) {
        output[i] = DQ[i].transform( input[i] );
    }
}

} // namespace SkinningPlugin

#endif // // RADIUM_ENGINE_DUAL_QUATERNION_SKINNING_HPP
