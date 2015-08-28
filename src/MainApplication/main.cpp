#include <MainApplication/MainApplication.hpp>

const uint FPS_MAX(120);
const Scalar DELTA_TIME( 1.0 / Scalar( FPS_MAX ) );

int main( int argc, char** argv )
{
    Ra::MainApplication app( argc, argv );

    Ra::Core::Timer::TimePoint t0, t1;

    while ( app.isRunning() )
    {
        t0 = Ra::Core::Timer::Clock::now();
        // Get events
        app.processEvents();

        // Main loop
        app.radiumFrame();

        // Wait for vsync

        Scalar remaining = DELTA_TIME;
        while (remaining > 0.0)
        {
            t1 = Ra::Core::Timer::Clock::now();
            remaining -= Ra::Core::Timer::getIntervalSeconds(t0, t1);
            t0 = t1;
        }
    }

    app.exit();
}
