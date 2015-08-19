#include <MainApplication/TimerData/FrameTimerData.hpp>

namespace Ra
{

#if 0
    LoggableFrameTimerData::LoggableFrameTimerData( uint average )
        : m_average( average )
    {
    }

    void LoggableFrameTimerData::addFrame( const FrameTimerData& data )
    {
        m_frames.push_back( data );
    }

    void LoggableFrameTimerData::log( el::base::type::ostream_t& os ) const
    {
        if ( m_average == 1 )
        {
            printTimerData( os );
        }
        else
        {
            printAverageTimerData( os );
        }

        m_frames.clear();
    }

    void LoggableFrameTimerData::printTimerData( el::base::type::ostream_t& os ) const
    {
        // Save stream state
        std::ios save( NULL );
        save.copyfmt( os );

        FrameTimerData data = m_frames[0];

        os << std::setprecision( 2 ) << std::fixed;

        os << "Timer data for frame " << data.numFrame << std::endl;
        {
            long renderRelStart = Core::Timer::getIntervalMicro( data.frameStart, data.renderData.renderStart );
            long renderRelEnd   = Core::Timer::getIntervalMicro( data.frameStart, data.renderData.renderEnd );
            long renderTime     = Core::Timer::getIntervalMicro( data.renderData.renderStart, data.renderData.renderEnd );
            Scalar fps    = 1.f / Core::Timer::getIntervalSeconds( data.renderData.renderStart, data.renderData.renderEnd );
            // FIXME(Charly): Assert disabled because of a difference of 1 that occurs sometimes
            //        CORE_ASSERT(renderTime == renderRelEnd - renderRelStart, "Timings are inconsitent");
            os << "\tRendering  : " << renderRelStart << " -> " << renderRelEnd << " : " << renderTime << " µs | "
               << fps << " updates per second" << std::endl;
        }
        {
            long tasksRelStart = Core::Timer::getIntervalMicro( data.frameStart, data.tasksStart );
            long tasksRelEnd   = Core::Timer::getIntervalMicro( data.frameStart, data.tasksEnd );
            long tasksTime     = Core::Timer::getIntervalMicro( data.tasksStart, data.tasksEnd );
            Scalar fps   = 1.f / Core::Timer::getIntervalSeconds( data.tasksStart, data.tasksEnd );
            // FIXME(Charly): Assert disabled because of a difference of 1 that occurs sometimes
            //        CORE_ASSERT(tasksTime == tasksRelEnd - tasksRelStart, "Timings are inconsitent");
            os << "\tTasks      : " << tasksRelStart << " -> " << tasksRelEnd << " : " << tasksTime << " µs | "
               << fps << " updates per second" << std::endl;
        }
        {
            long frameRelStart = Core::Timer::getIntervalMicro( data.frameStart, data.frameStart );
            long frameRelEnd =   Core::Timer::getIntervalMicro( data.frameStart, data.frameEnd );
            long frameTime =     Core::Timer::getIntervalMicro( data.frameStart, data.frameEnd );
            Scalar fps  =   1.f / Core::Timer::getIntervalSeconds( data.frameStart, data.frameEnd );
            os << "\tTotal time : " << frameRelStart << " -> " << frameRelEnd << " : " << frameTime << " µs | "
               << fps << " fps" << std::endl;
        }
        os << " =========   =========";

        // Restore stream state
        os.copyfmt( save );
    }

    void LoggableFrameTimerData::printAverageTimerData( el::base::type::ostream_t& os ) const
    {
        // Save stream state
        std::ios save( NULL );
        save.copyfmt( os );

        long sumRender = 0;
        long sumTasks = 0;
        long sumFrame = 0;
        long sumInterFrame = 0;
        for ( uint i = 0; i < m_frames.size(); ++i )
        {
            sumTasks += Core::Timer::getIntervalMicro( m_frames[i].tasksStart, m_frames[i].tasksEnd );
            sumRender += Core::Timer::getIntervalMicro( m_frames[i].renderData.renderStart, m_frames[i].renderData.renderEnd );
            sumFrame += Core::Timer::getIntervalMicro( m_frames[i].frameStart, m_frames[i].frameEnd );

            if ( i > 0 )
            {
                sumInterFrame +=  Core::Timer::getIntervalMicro( m_frames[i - 1].frameStart, m_frames[i].frameEnd );
            }
        }

        const uint N = m_frames.size();

        os << std::setprecision( 2 ) << std::fixed;
        os << "Average timer data : frames " << m_frames.front().numFrame << " to " << m_frames.back().numFrame << std::endl;
        os << "\tAverage render time : " << sumRender / N << " µs (" << N * 1000000.f / float(
               sumRender ) << " updates per second)" << std::endl;
        os << "\tAverage tasks time  : " << sumTasks / N << " µs (" << N * 1000000.f / float(
               sumTasks ) << " updates per second)" << std::endl;
        os << "\tAverage frame time  : " << sumFrame / N << " µs (" << N * 1000000.f / float(
               sumFrame ) << " max fps)" << std::endl;
        os << "\tAverage framerate   : " << ( N - 1 ) * 1000000.f / float( sumInterFrame ) << " fps";

        // Restore stream state
        os.copyfmt( save );

    }
#endif

}
