#ifndef RADIUMENGINE_FRAME_TIMER_DATA_HPP_
#define RADIUMENGINE_FRAME_TIMER_DATA_HPP_

#include <iostream>
#include <iomanip>

#include <Core/Log/Log.hpp>
#include <Core/Time/Timer.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra {

/// This structs holds all timings for one frame of the engine.
// FIXME(Charly): Make this loggable ?
struct FrameTimerData
{
    int numFrame;
    int timerAverage;
    Core::Timer::TimePoint frameStart;
    Core::Timer::TimePoint tasksStart;
    Core::Timer::TimePoint tasksEnd;
    Core::Timer::TimePoint frameEnd;
    Engine::Renderer::TimerData renderData;
    std::vector<Core::TaskQueue::TimerData> taskData;
};

class LoggableFrameTimerData : public el::Loggable
{
public:
    LoggableFrameTimerData(uint average);
    virtual ~LoggableFrameTimerData() {}
    void addFrame(const FrameTimerData& data);

    virtual void log(el::base::type::ostream_t& os) const override;

private:
    // Print timings relative to the beginning of the frame.
    void printTimerData(el::base::type::ostream_t& os) const;
    void printAverageTimerData(el::base::type::ostream_t& os) const;

private:
    mutable std::vector<FrameTimerData> m_frames;
    uint m_average;
};

}


#endif // RADIUMENGINE_FRAME_TIMER_DATA_HPP_
