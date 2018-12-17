#ifndef RADIUMENGINE_TIMER_HPP_
#define RADIUMENGINE_TIMER_HPP_

#include <Core/RaCore.hpp>
#include <chrono>

namespace Ra {
namespace Core {
// Convenience functions for timing.
// Wrapping the highest resolution time measuring.
namespace Utils {
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

inline long RA_CORE_API getIntervalMicro( const TimePoint& start, const TimePoint& end ) {
    return ( std::chrono::duration_cast<std::chrono::microseconds>( end - start ) ).count();
}

inline Scalar RA_CORE_API getIntervalSeconds( const TimePoint& start, const TimePoint& end ) {
    return ( std::chrono::duration_cast<
                 std::chrono::duration<Scalar, std::chrono::seconds::period>>( end - start ) )
        .count();
}

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_TIMER_HPP_
