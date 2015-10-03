#include <MainApplication/MainApplication.hpp>

#include <cstdio>



int main( int argc, char** argv )
{
    uint FPS_MAX = 120;
    
    if ( argc > 1 )
    {
        for ( int i = 1; i < argc; ++i )
        {
            std::string arg( argv[i] );
            if ( arg == "--fps" )
            {
                std::stringstream val( argv[i+1] );
                val >> FPS_MAX;

                break;
            }
        }
    }

    Scalar DELTA_TIME( FPS_MAX == 0 ? 0.0 : 1.0 / Scalar( FPS_MAX ) );
    
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
