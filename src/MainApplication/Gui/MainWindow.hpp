#ifndef RADIUMENGINE_MAINWINDOW_HPP
#define RADIUMENGINE_MAINWINDOW_HPP

#include <QMainWindow>
#include <ui_MainWindow.h>

namespace Ra
{

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

private:
    float m_fpsTimer;

private slots:
    void fpsCounter();
};

} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOW_HPP
