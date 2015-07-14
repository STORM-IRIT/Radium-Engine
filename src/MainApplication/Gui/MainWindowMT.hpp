#ifndef RADIUMENGINE_MAINWINDOWMT_HPP
#define RADIUMENGINE_MAINWINDOWMT_HPP

#include <QMainWindow>
#include <ui_MainWindowMT.h>

#include <qdebug.h>

namespace Ra
{

class MainWindowMT : public QMainWindow, private Ui::MainWindowMT
{
    Q_OBJECT

public:
    explicit MainWindowMT(QWidget* parent = nullptr);
    virtual ~MainWindowMT();

private:
};

} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOW_HPP
