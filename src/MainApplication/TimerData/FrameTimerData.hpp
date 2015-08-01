#ifndef RADIUMENGINE_FRAME_TIMER_DATA_HPP_
#define RADIUMENGINE_FRAME_TIMER_DATA_HPP_

#include<cstdio>

#include <Core/Time/Timer.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra{

/// This structs holds all timings for one frame of the engine.
struct FrameTimerData
{
    int numFrame;
    Core::Timer::TimePoint frameStart;
    Core::Timer::TimePoint tasksStart;
    Core::Timer::TimePoint tasksEnd;
    Core::Timer::TimePoint frameEnd;
    Engine::Renderer::TimerData renderData;
    std::vector<Core::TaskQueue::TimerData> taskData;


    // Print timings relative to the beginning of the frame.
    static void printTimerData(const FrameTimerData& data)
    {
        printf("Timer data for frame %i\n", data.numFrame);
        {
            long renderRelStart = Core::Timer::getIntervalMicro(data.frameStart, data.renderData.renderStart );
            long renderRelEnd =   Core::Timer::getIntervalMicro(data.frameStart, data.renderData.renderEnd);
            long renderTime =     Core::Timer::getIntervalMicro(data.renderData.renderStart, data.renderData.renderEnd);
            Scalar fps 	=   1.f / Core::Timer::getIntervalSeconds(data.renderData.renderStart, data.renderData.renderEnd);
            CORE_ASSERT(renderTime = renderRelStart - renderRelEnd, "Timings are inconsitent");
            printf("  Rendering : %lu -> %lu : %lu us | %2.1f /s\n", renderRelStart, renderRelEnd, renderTime, fps);
        }
        {
            long tasksRelStart = Core::Timer::getIntervalMicro(data.frameStart, data.tasksStart );
            long tasksRelEnd =   Core::Timer::getIntervalMicro(data.frameStart, data.tasksEnd );
            long tasksTime =     Core::Timer::getIntervalMicro(data.tasksStart, data.tasksEnd);
            Scalar fps 	=   1.f / Core::Timer::getIntervalSeconds(data.tasksStart, data.tasksEnd);
            CORE_ASSERT(tasksTime = tasksRelStart - tasksRelEnd, "Timings are inconsitent");
            printf("  Tasks : %lu -> %lu : %lu us | %2.1f /s\n", tasksRelStart, tasksRelEnd, tasksTime, fps);
        }
        {
            long frameRelStart = Core::Timer::getIntervalMicro(data.frameStart, data.frameStart );
            long frameRelEnd =   Core::Timer::getIntervalMicro(data.frameStart, data.frameEnd );
            long frameTime =     Core::Timer::getIntervalMicro(data.frameStart, data.frameEnd);
            Scalar fps 	=   1.f / Core::Timer::getIntervalSeconds(data.frameStart, data.frameEnd);
            printf("  Total time : %lu -> %lu : %lu us | %2.1f fps\n",frameRelStart, frameRelEnd, frameTime, fps);
        }
        printf(" =========   =========\n");
    }

    static void printAverageTimerData( const std::vector<FrameTimerData>& datas)
    {
        long sumRender = 0 ;
        long sumTasks =0 ;
        long sumFrame = 0;
        long sumInterFrame = 0;
        for (uint i =0; i < datas.size(); ++i)
        {
            sumTasks += Core::Timer::getIntervalMicro(datas[i].tasksStart, datas[i].tasksEnd);
            sumRender += Core::Timer::getIntervalMicro(datas[i].renderData.renderStart, datas[i].renderData.renderEnd);
            sumFrame += Core::Timer::getIntervalMicro(datas[i].frameStart, datas[i].frameEnd);

            if ( i >0 )
            {
                sumInterFrame +=  Core::Timer::getIntervalMicro(datas[i-1].frameStart, datas[i].frameEnd);
            }
        }
        const unsigned int N = datas.size();

        printf("Average timer data : frames %i to %i\n", datas.front().numFrame, datas.back().numFrame);
        printf("    Average render time : %lu us(%2.1f /s)\n", sumRender/N, N*1000000.f/float(sumRender));
        printf("    Average tasks time  : %lu us(%2.1f /s)\n", sumTasks/N, N*1000000.f/float(sumTasks));
        printf("    Average frame time  : %lu us(%2.1f max fps)\n", sumFrame/N, N*1000000.f/float(sumFrame));
        printf("    Average framerate   : %2.1f fps\n", (N-1)*1000000.f/float(sumInterFrame));

    }

};



}


#endif // RADIUMENGINE_FRAME_TIMER_DATA_HPP_
