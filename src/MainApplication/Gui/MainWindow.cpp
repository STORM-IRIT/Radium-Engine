#include <MainApplication/Gui/MainWindow.hpp>

#include <QApplication>
#include <QFileDialog>
#include <QMouseEvent>
#include <QKeyEvent>


#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <MainApplication/Gui/EntityTreeModel.hpp>

namespace Ra
{

Gui::MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    QStringList headers;
    headers << tr("Entities -> Components");

    m_entityTreeModel = new EntityTreeModel(headers);

    m_entitiesTreeView->setModel(m_entityTreeModel);

    createConnections();
}

Gui::MainWindow::~MainWindow()
{
    // Child QObjects will automatically be deleted
}

void Gui::MainWindow::createConnections()
{
    connect(actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile);
    connect(this, SIGNAL(entitiesUpdated(const std::vector<Engine::Entity*>&)),
            m_entityTreeModel, SLOT(entitiesUpdated(const std::vector<Engine::Entity*>&)));

    connect(m_entityTreeModel, SIGNAL(objectNameChanged(QString)),
            this, SLOT(objectNameChanged(QString)));
    connect(m_entityTreeModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
            m_entityTreeModel, SLOT(handleRename(QModelIndex, QModelIndex, QVector<int>)));

    connect(m_entitiesTreeView, SIGNAL(clicked(QModelIndex)), m_entityTreeModel, SLOT(handleSelect(QModelIndex)));

    connect(m_viewer, SIGNAL(entitiesUpdated()), this, SLOT(entitiesUpdated()));
    connect(m_viewer, SIGNAL(ready(Gui::Viewer*)), qApp, SLOT(viewerReady(Gui::Viewer*)));
}

void Gui::MainWindow::activated(QModelIndex index)
{
    fprintf(stderr, "Activated item %d %d\n", index.row(), index.column());
}

void Gui::MainWindow::clicked(QModelIndex index)
{
    fprintf(stderr, "Clicked item %d %d\n", index.row(), index.column());
}

void Gui::MainWindow::entitiesUpdated()
{
    //emit entitiesUpdated(m_viewer->getEngine()->getEntities());
}

void Gui::MainWindow::loadFile()
{
    bool res = false;
    QString path = QFileDialog::getOpenFileName(this, QString(), "..");
    if (path.size() > 0)
    {
//        res = m_viewer->loadFile(path);
    }

    if (res)
    {
//        emit entitiesUpdated(m_viewer->getEngine()->getEntities());
    }
}

void Gui::MainWindow::keyPressEvent(QKeyEvent * event)
{
    QMainWindow::keyPressEvent(event);
    m_keyEvents.push_back(keyEventQtToRadium(event));
}

void Gui::MainWindow::keyReleaseEvent(QKeyEvent * event)
{
    QMainWindow::keyReleaseEvent(event);
    m_keyEvents.push_back(keyEventQtToRadium(event));
}

Core::MouseEvent Gui::MainWindow::wheelEventQtToRadium(const QWheelEvent* qtEvent)
{
    Core::MouseEvent raEvent;
    raEvent.wheelDelta = qtEvent->delta();
    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
    }

    raEvent.absoluteXPosition = qtEvent->x();
    raEvent.absoluteYPosition = qtEvent->y();
    return raEvent;
}

Core::MouseEvent Gui::MainWindow::mouseEventQtToRadium(const QMouseEvent* qtEvent)
{
    Core::MouseEvent raEvent;
    switch (qtEvent->button())
    {
        case Qt::LeftButton:
        {
            raEvent.button = Core::MouseButton::RA_MOUSE_LEFT_BUTTON;
        } break;

        case Qt::MiddleButton:
        {
            raEvent.button = Core::MouseButton::RA_MOUSE_MIDDLE_BUTTON;
        } break;

        case Qt::RightButton:
        {
            raEvent.button = Core::MouseButton::RA_MOUSE_RIGHT_BUTTON;
        } break;

        default:
        {
        } break;
    }

    raEvent.modifier = 0;

    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
    }

    raEvent.absoluteXPosition = qtEvent->x();
    raEvent.absoluteYPosition = qtEvent->y();
    return raEvent;
}

Core::KeyEvent Gui::MainWindow::keyEventQtToRadium(const QKeyEvent* qtEvent)
{
    Core::KeyEvent raEvent;
    raEvent.key = qtEvent->key();

    raEvent.modifier = 0;

    if (qtEvent->modifiers().testFlag(Qt::ControlModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_CTRL_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::ShiftModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_SHIFT_KEY;
    }

    if (qtEvent->modifiers().testFlag(Qt::AltModifier))
    {
        raEvent.modifier |= Core::Modifier::RA_ALT_KEY;
    }
    return raEvent;
}


} // namespace Ra
