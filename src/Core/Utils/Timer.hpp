#ifndef RADIUMENGINE_TIMER_HPP_
#define RADIUMENGINE_TIMER_HPP_

#include <Core/RaCore.hpp>
#include <chrono>

namespace Ra {
namespace Core {
namespace Utils {

/** \name Time
 * Convenience functions for timing.
 * Wrapping the highest resolution time measuring.
 */
/// \{

/**
 * Type for a clock.
 */
using Clock = std::chrono::high_resolution_clock;

/**
 * Type for a point in time.
 */
using TimePoint = std::chrono::time_point<Clock>;

/**
 * Return the number of milliseconds between \p start and \p end.
 */
inline long getIntervalMicro( const TimePoint& start, const TimePoint& end ) {
    return ( std::chrono::duration_cast<std::chrono::microseconds>( end - start ) ).count();
}

/**
 * Return the number of seconds between \p start and \p end.
 */
inline Scalar getIntervalSeconds( const TimePoint& start, const TimePoint& end ) {
    return ( std::chrono::duration_cast<
                 std::chrono::duration<Scalar, std::chrono::seconds::period>>( end - start ) )
        .count();
}

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_TIMER_HPP_
