#ifndef RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP
#define RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP

#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/PoseOperation.hpp>
#include <Core/Math/Interpolation.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/** \name Predefined KeyFramedValue interpolators.
 * Linearly interpolates the keyframes right before and right after time \p t.
 * \note If there is no keyframe before \p t, then returns the first keyframe.
 * \note If there is no keyframe after \p t, then returns the last keyframe.
 */
/// \{
template <typename T>
inline T linearInterpolate( const KeyFramedValue<T>& keyframes, Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    auto [i, j, dt] = keyframes.findRange( t );
    return Core::Math::linearInterpolate( keyframes[i].second, keyframes[j].second, dt );
}

/// Force step for booleans.
template <>
inline bool linearInterpolate<bool>( const KeyFramedValue<bool>& keyframes, const Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    auto [i, j, dt] = keyframes.findRange( t );
    CORE_UNUSED( j );
    CORE_UNUSED( dt );
    return keyframes[i].second;
}

/// Force step for integers.
template <>
inline int linearInterpolate<int>( const KeyFramedValue<int>& keyframes, const Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    auto [i, j, dt] = keyframes.findRange( t );
    CORE_UNUSED( j );
    CORE_UNUSED( dt );
    return keyframes[i].second;
}

/// Specific implementation for Pose.
template <>
inline Pose linearInterpolate<Pose>( const KeyFramedValue<Pose>& keyframes, Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    auto [i, j, dt] = keyframes.findRange( t );
    return interpolatePoses( keyframes[i].second, keyframes[j].second, dt );
}
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP
