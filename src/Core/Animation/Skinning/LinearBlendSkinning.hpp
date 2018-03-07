#ifndef RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP
#define RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP

#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void RA_CORE_API linearBlendSkinning( const Vector3Array& inMesh, const Pose& pose,
                                      const WeightMatrix& weight, Vector3Array& outMesh );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP
