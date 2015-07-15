#include <MainApplication/Gui/MainWindow.hpp>

#include <thread>

#include <QApplication>

#include <Engine/Engine.hpp>
#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    connect(qApp, &QApplication::aboutToQuit, m_viewer, &Viewer::quit);
}

MainWindow::~MainWindow()
{
    // Child QObjects will automatically be deleted
}

} // namespace Ra
