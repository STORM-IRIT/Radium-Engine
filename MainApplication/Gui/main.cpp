#include <QApplication>

#include <Gui/MainWindow.hpp>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Ra::MainWindow w;
    w.show();
    return app.exec();
}
