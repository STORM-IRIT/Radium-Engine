#ifndef RADIUMENGINE_TIMER_HPP_
#define RADIUMENGINE_TIMER_HPP_

#include <Core/RaCore.hpp>
#include <chrono>

namespace Ra {
namespace Core {
// Convenience functions for timing.
// Wrapping the highest resolution time measuring.
namespace Utils {
using MicroSeconds = long;
using Seconds = Scalar;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

inline MicroSeconds getIntervalMicro( const TimePoint& start, const TimePoint& end ) {
    return ( std::chrono::duration_cast<std::chrono::microseconds>( end - start ) ).count();
}

inline Seconds getIntervalSeconds( const TimePoint& start, const TimePoint& end ) {
    return ( std::chrono::duration_cast<
                 std::chrono::duration<Scalar, std::chrono::seconds::period>>( end - start ) )
        .count();
}
} // namespace Utils

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_TIMER_HPP_
