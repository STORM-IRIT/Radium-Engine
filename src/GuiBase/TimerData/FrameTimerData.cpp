#include <GuiBase/TimerData/FrameTimerData.hpp>

namespace Ra
{
    void FrameTimerData::print(std::ostream& ostream) const
    {

        long totalTime = Ra::Core::Timer::getIntervalMicro(frameStart, frameEnd);

        ostream<<"frame "<< numFrame <<": "<<totalTime<<"\n";
        ostream<<"{"<<"\n";
        {
            long evStart = Ra::Core::Timer::getIntervalMicro(frameStart, eventsStart);
            long evEnd = Ra::Core::Timer::getIntervalMicro(frameStart, eventsEnd);

            long taStart = Ra::Core::Timer::getIntervalMicro(frameStart, tasksStart);
            long taEnd = Ra::Core::Timer::getIntervalMicro(frameStart, tasksEnd);

            long reStart = Ra::Core::Timer::getIntervalMicro(frameStart, renderData.renderStart);
            long reEnd = Ra::Core::Timer::getIntervalMicro(frameStart, renderData.renderEnd);
            ostream << "\tevents: " << evStart << " " << evEnd << " " << evEnd - evStart << "\n";
            ostream << "\ttasks: " << taStart << " " << taEnd << " " << taEnd - taStart << "\n";
            ostream << "\t{" << "\n";
            for (const auto& tData : taskData)
            {
                long tadaStart = Ra::Core::Timer::getIntervalMicro(frameStart, tData.start);
                long tadaEnd = Ra::Core::Timer::getIntervalMicro(frameStart, tData.end);
                ostream << "\t\t" << tData.taskName << "("<<tData.threadId<<"): " << tadaStart << " " << tadaEnd << " " << tadaEnd - tadaStart
                        << "\n";
            }
            ostream << "\t}" << "\n";
            ostream << "\trender: " << reStart << " " << reEnd << " " << reEnd - reStart << "\n";
        }
        ostream<<"}"<<"\n";
        ostream<<std::endl;
    }
}
