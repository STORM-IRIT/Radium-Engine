#ifndef RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP
#define RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP

#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Animation/Pose.hpp>
#include <Core/Animation/PoseOperation.hpp>
#include <Core/Math/Interpolation.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * Look for the keyframes around time \p t.
 * \param[in] keyframes The list of keyframes.
 * \param[in] t The time to search for.
 * \param[out] i0 The index for the keyframe preceding t.
 * \param[out] i1 The index for the keyframe following t.
 * \param[out] dt Linear parameter, \f$ \in [0;1] \f$, of \p t between times \p t0
 *             and \p t1 at keyframes i0 and i1.
 * \note If \p t is lower than the first keyframe's time tf,
 *       then \p i0 = \p i1 = 0 and \p dt = 0.
 *       If \p t is higher than the last keyframe's time tl,
 *       then \p i0 = \p i1 = the index of the last keyframe and \p dt = 0.
 *       If \p t is an exact match on a keyframe's time tt,
 *       then \p i0 = \p i1 = the index of the keyframe and \p dt = 0.
 */
template <typename T>
void findRange( const typename KeyFramedValue<T>::KeyFrames& keyframes,
                const Scalar& t,
                size_t& i0,
                size_t& i1,
                Scalar& dt ) {
    // before first
    if ( t < keyframes.begin()->first )
    {
        i0 = 0;
        i1 = i0;
        dt = 0_ra;
        return;
    }
    // after last
    if ( t > keyframes.rbegin()->first )
    {
        i0 = keyframes.size() - 1;
        i1 = i0;
        dt = 0_ra;
        return;
    }
    // look for exact match
    auto kf0   = keyframes[0];
    kf0.first  = t;
    auto upper = std::upper_bound(
        keyframes.begin(), keyframes.end(), kf0, []( const auto& a, const auto& b ) {
            return a.first < b.first;
        } );
    auto lower = upper;
    --lower;
    if ( Math::areApproxEqual( lower->first, t ) )
    {
        i0 = std::distance( keyframes.begin(), lower );
        i1 = i0;
        dt = 0_ra;
        return;
    }
    // in-between
    i0        = std::distance( keyframes.begin(), lower );
    i1        = std::distance( keyframes.begin(), upper );
    Scalar t0 = lower->first;
    Scalar t1 = upper->first;
    dt        = ( t - t0 ) / ( t1 - t0 );
}

/** \name Predefined KeyFramedValue interpolators.
 * Linearly interpolates the keyframes right before and right after time \p t.
 * \note If there is no keyframe before \p t, then returns the first keyframe.
 * \note If there is no keyframe after \p t, then returns the last keyframe.
 */
/// \{
template <typename T>
inline T linearInterpolate( const typename KeyFramedValue<T>::KeyFrames& keyframes, Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    size_t i0, i1;
    Scalar dt;
    findRange<T>( keyframes, t, i0, i1, dt );
    return Core::linearInterpolate( keyframes[i0].second, keyframes[i1].second, dt );
}

/// Step for booleans.
template <>
inline bool linearInterpolate<bool>( const KeyFramedValue<bool>::KeyFrames& keyframes,
                                     const Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    size_t i0, i1;
    Scalar dt;
    findRange<bool>( keyframes, t, i0, i1, dt );
    return keyframes[i0].second;
}

/// Step for integers.
template <>
inline int linearInterpolate<int>( const KeyFramedValue<int>::KeyFrames& keyframes,
                                   const Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    size_t i0, i1;
    Scalar dt;
    findRange<int>( keyframes, t, i0, i1, dt );
    return keyframes[i0].second;
}

template <>
inline Pose linearInterpolate<Pose>( const KeyFramedValue<Pose>::KeyFrames& keyframes, Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    size_t i0, i1;
    Scalar dt;
    findRange<Pose>( keyframes, t, i0, i1, dt );
    return interpolatePoses( keyframes[i0].second, keyframes[i1].second, dt );
}
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP
