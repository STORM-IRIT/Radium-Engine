#include <MainApplication/Gui/MainWindow.hpp>

namespace Ra
{

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);
}

MainWindow::~MainWindow()
{
    // Child QObjects will automatically be deleted
}

} // namespace Ra
