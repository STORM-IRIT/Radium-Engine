#include <qapplication.h>
#include <MainApplication/Gui/MainWindow.hpp>

#include <Core/CoreMacros.hpp>

#ifdef OS_LINUX
#include <X11/Xlib.h>
#endif


int main(int argc, char** argv)
{
#ifdef OS_LINUX
    XInitThreads();
#endif
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setVersion(4, 4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(0);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(16);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(format);

    Ra::MainWindow w;
    w.show();
    return app.exec();
}
