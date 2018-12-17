#ifndef RADIUMENGINE_FRAME_TIMER_DATA_HPP
#define RADIUMENGINE_FRAME_TIMER_DATA_HPP

#include <GuiBase/RaGuiBase.hpp>

#include <iomanip>
#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Timer.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra {

/// This struct holds all timings for one frame of the engine.
struct RA_GUIBASE_API FrameTimerData {
    uint numFrame;
    Core::Utils::TimePoint frameStart;
    Core::Utils::TimePoint eventsStart;
    Core::Utils::TimePoint eventsEnd;
    Core::Utils::TimePoint tasksStart;
    Core::Utils::TimePoint tasksEnd;
    Core::Utils::TimePoint frameEnd;
    Engine::Renderer::TimerData renderData;
    std::vector<Core::TaskQueue::TimerData> taskData;

    void print( std::ostream& ostream ) const;
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
