#ifndef RADIUMENGINE_MAINWINDOW_HPP
#define RADIUMENGINE_MAINWINDOW_HPP

#include <QMainWindow>
#include <ui_MainWindow.h>

#include <qdebug.h>

namespace Ra { namespace Engine { class Entity; } }
namespace Ra { namespace Gui { class EntityTreeModel; } }

namespace Ra { namespace Gui {

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
    void clicked(QModelIndex index);
    void activated(QModelIndex index);

public slots:
    void entitiesUpdated();

signals:
    void entitiesUpdated(const std::vector<Engine::Entity*>&);

private:
    void updateEntitiesTree();

private:
    EntityTreeModel* m_entityTreeModel;
};

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_MAINWINDOW_HPP
