#ifndef ANIMPLUGIN_SKELETON_UTILS_HPP_
#define ANIMPLUGIN_SKELETON_UTILS_HPP_

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Animation {
class Skeleton;
/// Returns the start and end point of a bone in model space.
RA_CORE_API void getBonePoints( const Skeleton& skeleton, int boneIdx,
                                Ra::Core::Math::Vector3& startOut,
                                Ra::Core::Math::Vector3& endOut );

/// Gives out the nearest point on a given bone.
RA_CORE_API Ra::Core::Math::Vector3 projectOnBone( const Skeleton& skeleton, int boneIdx,
                                                   const Ra::Core::Math::Vector3& pos );

RA_CORE_API void to_string( const Skeleton& skeleton );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif
