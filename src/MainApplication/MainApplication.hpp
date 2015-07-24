#include <Core/CoreMacros.hpp>

#include <iostream>
#include <QApplication>
#include <QTime>

#include <MainApplication/Gui/MainWindow.hpp>


namespace Ra
{
    class MainApplication : public QApplication
    {
    public:
        MainApplication(int argc, char** argv) : QApplication(argc, argv)
        {
            // Boilerplate print.

            std::cerr << "*** Radium Engine Main App  ***" << std::endl;
#if defined (CORE_DEBUG)
            std::cerr << "(Debug Build) -- ";
#else
            std::cerr<<"(Release Build) -- ";
#endif

#if defined (ARCH_X86)
            std::cerr<<" 32 bits x86";
#elif defined (ARCH_X64)
            std::cerr << " 64 bits x64";
#endif
            std::cerr << std::endl;

            std::cerr << "Floating point format : ";
#if defined(CORE_USE_DOUBLE)
            std::cerr<<"double precision"<<std::endl;
#else
            std::cerr << "single precision" << std::endl;
#endif


            // Handle command line arguments.


            // Create default format for Qt.
            QSurfaceFormat format;
            format.setVersion(4, 4);
            format.setProfile(QSurfaceFormat::CoreProfile);
            format.setSamples(0);
            format.setDepthBufferSize(24);
            format.setStencilBufferSize(8);
            format.setSamples(16);
            format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
            QSurfaceFormat::setDefaultFormat(format);

            // Create main window.
            m_mainWindow.reset ( new Gui::MainWindow );
            m_mainWindow->show();
        }



    private:
        std::unique_ptr<Gui::MainWindow> m_mainWindow;
    };

}