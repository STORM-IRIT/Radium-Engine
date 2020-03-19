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
 * \param[out] t0 The time for the keyframe preceding t.
 * \param[out] t1 The time for the keyframe following t.
 * \param[out] dt Linear parameter, \f$ \in [0;1] \f$, of \p t between \p t0 and \p t1.
 * \note If \p t is lower than the first keyframe's time tf,
 *       then \p t0 = \p t1 = tf and \p dt = 0.
 *       If \p t is higher than the last keyframe's time tl,
 *       then \p t0 = \p t1 = tl and \p dt = 0.
 *       If \p t is an exact match on a keyframe's time tt,
 *       then \p t0 = \p t1 = tt and \p dt = 0.
 */
template <typename T>
void findRange( const typename KeyFramedValue<T>::KeyFrames& keyframes,
                const Scalar& t,
                Scalar& t0,
                Scalar& t1,
                Scalar& dt ) {
    // before first
    if ( t < keyframes.begin()->first )
    {
        t0 = keyframes.begin()->first;
        t1 = t0;
        dt = 0.0;
        return;
    }
    // after last
    if ( t > keyframes.rbegin()->first )
    {
        t0 = keyframes.rbegin()->first;
        t1 = t0;
        dt = 0.0;
        return;
    }
    auto it = keyframes.find( t );
    // exact match
    if ( it != keyframes.end() )
    {
        t0 = it->first;
        t1 = t0;
        dt = 0.0;
        return;
    }
    // in-between
    auto upper = keyframes.upper_bound( t );
    auto lower = upper;
    --lower;
    t0 = lower->first;
    t1 = upper->first;
    dt = ( t - t0 ) / ( t1 - t0 );
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
    Scalar t0, t1, dt;
    findRange<T>( keyframes, t, t0, t1, dt );
    return Core::linearInterpolate( keyframes.at( t0 ), keyframes.at( t1 ), dt );
}

/// Step for booleans.
template <>
inline bool linearInterpolate<bool>( const KeyFramedValue<bool>::KeyFrames& keyframes,
                                     const Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    Scalar t0, t1, dt;
    findRange<bool>( keyframes, t, t0, t1, dt );
    return keyframes.at( t0 );
}

/// Step for integers.
template <>
inline int linearInterpolate<int>( const KeyFramedValue<int>::KeyFrames& keyframes,
                                   const Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    Scalar t0, t1, dt;
    findRange<int>( keyframes, t, t0, t1, dt );
    return keyframes.at( t0 );
}

template <>
inline Pose linearInterpolate<Pose>( const KeyFramedValue<Pose>::KeyFrames& keyframes, Scalar t ) {
    CORE_ASSERT( keyframes.size() > 0, "Keyframe vectors must contain at least one keyframe." );
    Scalar t0, t1, dt;
    findRange<Pose>( keyframes, t, t0, t1, dt );
    return interpolatePoses( keyframes.at( t0 ), keyframes.at( t1 ), dt );
}
/// \}

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEYFRAMEDVALUE_INTERPOLATOR_HPP
