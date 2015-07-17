#include <iostream>
#include <QApplication>
#include <Core/CoreMacros.hpp>
#include <MainApplication/Gui/MainWindow.hpp>

int main(int argc, char** argv)
{
    std::cerr<<"*** Radium Engine Main App  ***"<<std::endl;
#if defined (CORE_DEBUG)
    std::cerr<<"(Debug Build)"<<std::endl;
 #else
    std::cerr<<"(Release Build)"<<std::endl;
#endif
    std::cerr<<"Floating point format : ";
#if defined(CORE_USE_DOUBLE)
    std::cerr<<"double precision"<<std::endl;
#else
    std::cerr<<"single precision"<<std::endl;
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
