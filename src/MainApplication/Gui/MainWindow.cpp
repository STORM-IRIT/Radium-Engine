#include <MainApplication/Gui/MainWindow.hpp>

#include <thread>

#include <QApplication>
#include <QFileDialog>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderSystem.hpp>
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
    connect(qApp, &QApplication::aboutToQuit, m_viewer, &Viewer::quit);

    connect(actionOpenMesh, &QAction::triggered, this, &MainWindow::loadFile);
    connect(this, SIGNAL(entitiesUpdated(const std::vector<Engine::Entity*>&)),
            m_entityTreeModel, SLOT(entitiesUpdated(const std::vector<Engine::Entity*>&)));

    connect(m_entityTreeModel, SIGNAL(objectNameChanged(QString)),
            this, SLOT(objectNameChanged(QString)));
    connect(m_entityTreeModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
            m_entityTreeModel, SLOT(handleRename(QModelIndex, QModelIndex, QVector<int>)));

    connect(m_entitiesTreeView, SIGNAL(clicked(QModelIndex)), m_entityTreeModel, SLOT(handleSelect(QModelIndex)));

    connect(m_viewer, SIGNAL(entitiesUpdated()), this, SLOT(entitiesUpdated()));
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
    emit entitiesUpdated(m_viewer->getEngine()->getEntities());
}

void Gui::MainWindow::loadFile()
{
    bool res = false;
    QString path = QFileDialog::getOpenFileName(this, QString(), "..");
    if (path.size() > 0)
    {
        res = m_viewer->loadFile(path);
    }

    if (res)
    {
        emit entitiesUpdated(m_viewer->getEngine()->getEntities());
    }
}

} // namespace Ra
