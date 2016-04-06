#ifndef RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP
#define RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void linearBlendingSkinning( const Vector3Array&  inMesh,
                             const Pose&          pose,
                             const WeightMatrix&  weight,
                             Vector3Array&        outMesh );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP

