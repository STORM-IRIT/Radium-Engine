#ifndef RADIUMENGINE_MAINWINDOW_HPP
#define RADIUMENGINE_MAINWINDOW_HPP

#include <QMainWindow>
#include <ui_MainWindow.h>

#include <qdebug.h>

namespace Ra
{

// Just forwards everything to the viewer.
class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

    void createConnections();

private slots:
    void loadFile();

private:

};

} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOW_HPP
