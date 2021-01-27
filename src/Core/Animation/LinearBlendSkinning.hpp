#pragma once

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

void RA_CORE_API linearBlendSkinning( const Vector3Array& inMesh,
                                      const Pose& pose,
                                      const WeightMatrix& weight,
                                      Vector3Array& outMesh );

} // namespace Animation
} // namespace Core
} // namespace Ra
