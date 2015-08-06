#ifndef RADIUMENGINE_TIMER_HPP_
#define RADIUMENGINE_TIMER_HPP_

#include <Core/CoreMacros.hpp>
#include <chrono>

namespace Ra
{
    namespace Core
    {
        // Convenience functions for timing.
        // Wrapping the highest resolution time measuring.
        namespace Timer
        {
            typedef std::chrono::high_resolution_clock  Clock;
            typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

            inline long getIntervalMicro ( const TimePoint& start, const TimePoint& end )
            {
                return ( std::chrono::duration_cast<std::chrono::microseconds> ( end - start ) ).count();
            }

            inline Scalar getIntervalSeconds ( const TimePoint& start, const TimePoint& end )
            {
                return ( std::chrono::duration_cast<std::chrono::duration<Scalar, std::chrono::seconds::period>>
                         ( end - start ) ).count();
            }
        }

    }
}


#endif// RADIUMENGINE_TIMER_HPP_
