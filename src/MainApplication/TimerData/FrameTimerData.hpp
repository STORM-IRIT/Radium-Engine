#ifndef RADIUMENGINE_FRAME_TIMER_DATA_HPP
#define RADIUMENGINE_FRAME_TIMER_DATA_HPP

#include <iostream>
#include <iomanip>

#include <Core/Log/Log.hpp>
#include <Core/Time/Timer.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra
{

    /// This struct holds all timings for one frame of the engine.
    struct FrameTimerData
    {
        int numFrame;
        Core::Timer::TimePoint frameStart;
        Core::Timer::TimePoint eventsStart;
        Core::Timer::TimePoint eventsEnd;
        Core::Timer::TimePoint tasksStart;
        Core::Timer::TimePoint tasksEnd;
        Core::Timer::TimePoint frameEnd;
        Engine::Renderer::TimerData renderData;
        std::vector<Core::TaskQueue::TimerData> taskData;
    };

#if 0
    class LoggableFrameTimerData
    {
    public:
        LoggableFrameTimerData( uint average );
        virtual ~LoggableFrameTimerData() {}
        void addFrame( const FrameTimerData& data );

        //void log(el::base::type::ostream_t& os) const override;

    private:
        // Print timings relative to the beginning of the frame.
        void printTimerData( el::base::type::ostream_t& os ) const;
        void printAverageTimerData( el::base::type::ostream_t& os ) const;

    private:
        mutable std::vector<FrameTimerData> m_frames;
        uint m_average;
    };
#endif

}


#endif // RADIUMENGINE_FRAME_TIMER_DATA_HPP_
