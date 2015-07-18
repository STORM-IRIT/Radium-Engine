#include <MainApplication/Gui/MainWindow.hpp>

#include <thread>

#include <QApplication>
#include <QFileDialog>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    createConnections();
}

MainWindow::~MainWindow()
{
    // Child QObjects will automatically be deleted
}

void MainWindow::createConnections()
{
    connect(qApp, &QApplication::aboutToQuit, m_viewer, &Viewer::quit);

    connect(actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile);
}

void MainWindow::loadFile()
{
    QString path = QFileDialog::getOpenFileName(this, QString(), "..");
    if (path.size() > 0)
    {
        m_viewer->loadFile(path);
    }
}

} // namespace Ra
