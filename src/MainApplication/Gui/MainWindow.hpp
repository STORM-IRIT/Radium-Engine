#ifndef RADIUMENGINE_MAINWINDOW_HPP
#define RADIUMENGINE_MAINWINDOW_HPP

#include <QMainWindow>
#include <ui_MainWindow.h>

#include <qdebug.h>

namespace Ra
{

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

private:
};

} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOW_HPP
