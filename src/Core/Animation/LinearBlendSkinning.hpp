#ifndef RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP
#define RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/// \name Linear Blend Skinning
/// \{

/**
 * Apply the linear interpolation of transformations to the vertices of \p inMesh.
 */
RA_CORE_API void linearBlendSkinning( const Vector3Array& inMesh, const Pose& pose,
                                      const WeightMatrix& weight, Vector3Array& outMesh );
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_LINEAR_BLENDING_SKINNING_HPP
