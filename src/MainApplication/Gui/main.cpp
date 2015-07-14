#include <qapplication.h>
#include <MainApplication/Gui/MainWindow.hpp>
#include <MainApplication/Gui/MainWindowMT.hpp>

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
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
    format.setSamples(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

//#define WITH_MT
#ifdef WITH_MT
    Ra::MainWindowMT w;
#else
    Ra::MainWindow w;
#endif
    w.show();
    return app.exec();
}
