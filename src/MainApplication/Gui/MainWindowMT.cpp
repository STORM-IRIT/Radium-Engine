#include <MainApplication/Gui/MainWindowMT.hpp>

namespace Ra
{

MainWindowMT::MainWindowMT(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);
}

MainWindowMT::~MainWindowMT()
{
    // Child QObjects will automatically be deleted
}

} // namespace Ra
