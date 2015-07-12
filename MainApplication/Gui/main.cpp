#include <QApplication>

#include <Gui/MainWindow.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setVersion(4, 4);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	QSurfaceFormat::setDefaultFormat(format);

    Ra::MainWindow w;
    w.show();
    return app.exec();
}
