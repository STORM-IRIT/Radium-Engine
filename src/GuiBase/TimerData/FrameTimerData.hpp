#ifndef RADIUMENGINE_FRAME_TIMER_DATA_HPP
#define RADIUMENGINE_FRAME_TIMER_DATA_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <iomanip>
#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Time/Timer.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra {

/// This struct holds all timings for one frame of the engine.
struct RA_GUIBASE_API FrameTimerData {
    uint numFrame;                                    ///< Frame number.
    Core::Timer::TimePoint frameStart;                ///< When did the frame start.
    Core::Timer::TimePoint eventsStart;               ///< When did the events processing start.
    Core::Timer::TimePoint eventsEnd;                 ///< When did the events processing end.
    Core::Timer::TimePoint tasksStart;                ///< When did the tasks processing start.
    Core::Timer::TimePoint tasksEnd;                  ///< When did the tasks processing end.
    Core::Timer::TimePoint frameEnd;                  ///< When did the frame end.
    Engine::Renderer::TimerData renderData;           ///< Rendering timings.
    std::vector<Core::TaskQueue::TimerData> taskData; ///< Tasks processing timings.

    void print( std::ostream& ostream ) const; ///< Print stat info to the given stream.
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

} // namespace Ra

#endif // RADIUMENGINE_FRAME_TIMER_DATA_HPP_
