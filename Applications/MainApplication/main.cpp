#include <MainApplication.hpp>

#include <QCommandLineParser>

#include <GuiBase/Utils/KeyMappingManager.hpp>

#include <Gui/MainWindow.hpp>


class MainWindowFactory:public Ra::GuiBase::BaseApplication::WindowFactory{
public:
    using Ra::GuiBase::BaseApplication::WindowFactory::WindowFactory;
    Ra::GuiBase::MainWindowInterface *createMainWindow() const
    {
        return new Ra::Gui::MainWindow();
    }
};

int main( int argc, char** argv )
{
    Ra::MainApplication app( argc, argv, MainWindowFactory() );

    const uint& fpsMax = app.m_targetFPS;
    const Scalar deltaTime( fpsMax == 0 ? 0.f : 1.f / Scalar( fpsMax ) );

    Ra::Core::Timer::TimePoint t0, t1;

    while ( app.isRunning() )
    {
        t0 = Ra::Core::Timer::Clock::now();
        // Main loop
        app.radiumFrame();

        // Wait for VSync
        Scalar remaining = deltaTime;
        while (remaining > 0.0)
        {
            t1 = Ra::Core::Timer::Clock::now();
            remaining -= Ra::Core::Timer::getIntervalSeconds(t0, t1);
            t0 = t1;
        }
    }

    app.exit();
}
