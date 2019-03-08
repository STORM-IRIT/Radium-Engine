#ifndef RADIUMENGINE_DUAL_QUATERNION_SKINNING_HPP
#define RADIUMENGINE_DUAL_QUATERNION_SKINNING_HPP

#include <Core/Animation/HandleWeight.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * Defines the type for lists of DualQuaternions.
 */
using DQList = AlignedStdVector<DualQuaternion>;

/// \name Dual Quaternion Skinning
/// \{

/**
 * Computes the dual quaternions from a given pose and a given set of skinning weights.
 * \warning \p pose and \p weight must have the same size.
 */
RA_CORE_API void computeDQ( const Pose& pose, const WeightMatrix& weight, DQList& DQ );

/// Same version, without the parallelism for reference purposes (see github issue #118).
RA_CORE_API void computeDQ_naive( const Pose& pose, const WeightMatrix& weight, DQList& DQ );

/**
 * Applies a set of dual quaternions to a given input set of vertices and
 * returns the resulting transformed vertices.
 * \warning \p pose and \p weight must have the same size.
 */
RA_CORE_API void dualQuaternionSkinning( const Ra::Core::Vector3Array& input, const DQList& DQ,
                                         Ra::Core::Vector3Array& output );
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DUAL_QUATERNION_SKINNING_HPP
