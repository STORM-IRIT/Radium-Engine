#include <QCommandLineParser>

#include <MainApplication/MainApplication.hpp>

#include <cstdio>

int main( int argc, char** argv )
{
    Ra::MainApplication app( argc, argv );

    uint FPS_MAX = app.m_targetFPS;
    Scalar DELTA_TIME( FPS_MAX == 0 ? 0.0 : 1.0 / Scalar( FPS_MAX ) );

    Ra::Core::Timer::TimePoint t0, t1;

    while ( app.isRunning() )
    {
        t0 = Ra::Core::Timer::Clock::now();
        // Main loop
        app.radiumFrame();

        // Wait for VSync
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
