#ifndef RADIUMENGINE_DUAL_QUATERNION_SKINNING_HPP
#define RADIUMENGINE_DUAL_QUATERNION_SKINNING_HPP

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/DualQuaternion.hpp>
#include <Core/Animation/Pose/Pose.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
namespace Core {
namespace Animation {

typedef AlignedStdVector< DualQuaternion > DQList;

void RA_CORE_API computeDQ( const Pose& pose, const WeightMatrix& weight, DQList& DQ );
void RA_CORE_API DualQuaternionSkinning( const Vector3Array& input, const DQList& DQ, Vector3Array& output );

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DUAL_QUATERNION_SKINNING_HPP
