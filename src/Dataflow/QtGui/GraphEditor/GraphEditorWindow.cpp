#include "ui_NodeEditor.h"

#include <Dataflow/QtGui/GraphEditor/GraphEditorWindow.hpp>

#include <Dataflow/Core/NodeFactory.hpp>

#include <memory>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {

using namespace Ra::Dataflow::Core;

GraphEditorWindow::~GraphEditorWindow() {}

GraphEditorWindow::GraphEditorWindow( std::shared_ptr<DataflowGraph> graph ) : m_graph { graph } {

    if ( !m_graph ) { m_graph = std::make_shared<DataflowGraph>( "" ); }

    auto central_widget = new QWidget( this );
    auto central_layout = new QVBoxLayout( central_widget );

    m_graph_model = std::make_shared<GraphModel>( m_graph );
    m_scene       = new QtNodes::BasicGraphicsScene( *m_graph_model, this );
    m_view        = new QtNodes::GraphicsView( m_scene, this );

    central_layout->addWidget( m_view );
    central_layout->setContentsMargins( 0, 0, 0, 0 );
    central_layout->setSpacing( 0 );
    setCentralWidget( central_widget );
    central_widget->setFocusPolicy( Qt::StrongFocus );

    createActions();
    createStatusBar();
    readSettings();

    setCurrentFile( QString() );
    setUnifiedTitleAndToolBarOnMac( true );

    QDockWidget* dock = new QDockWidget( this );
    dock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    auto node_tree_widget = new QTreeWidget( dock );
    node_tree_widget->setColumnCount( 1 );
    node_tree_widget->setHeaderLabel( "Nodes" );

    for ( const auto& [factory_name, factory] : NodeFactoriesManager::getFactoryManager() ) {

        std::map<std::string, std::vector<std::string>> node_list;
        for ( const auto& [model_name, creator] : factory->getFactoryMap() ) {
            auto f              = creator.first;
            auto creatorFactory = factory;
            node_list[creator.second].push_back( model_name );
        }

        auto factory_item =
            new QTreeWidgetItem( QStringList() << QString::fromStdString( factory_name ) );

        for ( auto [category, model_names] : node_list ) {
            auto n = new QTreeWidgetItem( QStringList() << QString::fromStdString( category ) );
            std::sort( model_names.begin(), model_names.end() );
            for ( const auto& m : model_names ) {
                n->addChild( new QTreeWidgetItem( QStringList() << QString::fromStdString( m ) ) );
            }

            factory_item->addChild( n );
        }
        node_tree_widget->addTopLevelItem( factory_item );
    }

    connect( node_tree_widget, &QTreeWidget::itemDoubleClicked, this, &GraphEditorWindow::addNode );
    connect( m_graph_model.get(), &GraphModel::node_edited, this, &GraphEditorWindow::node_editor );
    connect( m_scene,
             &QtNodes::BasicGraphicsScene::nodeDoubleClicked,
             this,
             &GraphEditorWindow::node_dialog );

    dock->setWidget( node_tree_widget );
    addDockWidget( Qt::LeftDockWidgetArea, dock );
    viewMenu->addAction( dock->toggleViewAction() );
}

void GraphEditorWindow::addNode( QTreeWidgetItem* item, int ) {
    if ( item->childCount() == 0 ) { m_graph_model->addNode( item->text( 0 ) ); }
}

class GraphEditorDialog : public QDialog
{
  public:
    GraphEditorDialog( std::shared_ptr<DataflowGraph> graph, QWidget* parent ) : QDialog( parent ) {
        auto p_dialogLayout = new QGridLayout( this );
        auto p_MainWindow   = new GraphEditorWindow( graph );
        auto model          = p_MainWindow->graph_model();
        model->addInputOutputNodesForGraph();
        p_dialogLayout->addWidget( p_MainWindow );
        p_MainWindow->setParent( this );
    }
};

void GraphEditorWindow::node_editor( std::shared_ptr<Node> node ) {
    auto g = std::dynamic_pointer_cast<DataflowGraph>( node );
    auto w = new GraphEditorDialog( g, this );

    w->setWindowModality( Qt::ApplicationModal );
    w->show();
    connect( w, &GraphEditorDialog::finished, [this, g]( int ) {
        g->generate_ports();
        m_graph_model->clear_node_widget( g.get() );
        m_graph_model->sync_data();
    } );
}

class NodeEditorDialog : public QDialog
{
  public:
    NodeEditorDialog( Node* node, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::Dialog ) :
        QDialog( parent, f ), m_node { node } {
        ui.setupUi( this );

        {
            ui.node_model->setText( QString::fromStdString( node->getModelName() ) );
            ui.node_instance->setText( QString::fromStdString( node->getInstanceName() ) );
            ui.display_name_line_edit->setText( QString::fromStdString( node->display_name() ) );
            data_editors.emplace_back( ui.display_name_line_edit, [node]( const QString& text ) {
                node->set_display_name( text.toStdString() );
            } );
        }

        auto layout = ui.port_grid_layout;
        int row     = 2;

        for ( auto p : node->getInputs() ) {
            auto l = new QLineEdit( QString::fromStdString( p->getName() ) );
            data_editors.emplace_back(
                l, [p]( const QString& text ) { p->setName( text.toStdString() ); } );

            layout->addWidget( new QLabel( QString::fromStdString( p->getName() ) ), row, 0 );
            layout->addWidget( l, row, 1 );

            ++row;
        }
        row = 2;
        for ( auto p : node->getOutputs() ) {
            auto l = new QLineEdit( QString::fromStdString( p->getName() ) );
            data_editors.emplace_back(
                l, [p]( const QString& text ) { p->setName( text.toStdString() ); } );

            layout->addWidget( new QLabel( QString::fromStdString( p->getName() ) ), row, 2 );
            layout->addWidget( l, row, 3 );

            ++row;
        }
        auto graph_node = dynamic_cast<GraphNode*>( node );
        if ( graph_node ) {
            clear_unused = new QCheckBox( "clear unused" );
            layout->addWidget( clear_unused, row, 0 );
        }
        connect( ui.buttonBox, &QDialogButtonBox::accepted, this, &NodeEditorDialog::run );
    }

  public slots:
    void run() {
        for ( auto p : data_editors ) {
            p.second( p.first->displayText() );
        }
        if ( clear_unused && clear_unused->checkState() == Qt::Checked ) {
            auto graph_node = dynamic_cast<GraphNode*>( m_node );
            if ( graph_node ) { graph_node->remove_unlinked_ports(); }
        }
    }

  private:
    std::vector<std::pair<QLineEdit*, std::function<void( const QString& )>>> data_editors;
    Node* m_node { nullptr };
    QCheckBox* clear_unused { nullptr };
    Ui::NodeEditor ui;
};

void GraphEditorWindow::node_dialog( QtNodes::NodeId node_id ) {
    auto node = m_graph_model->node_ptr( node_id );

    NodeEditorDialog dialog( node.get() );
    //    int ret =
    dialog.exec(); // exec is blocking main window, as expected for a modal
    m_graph_model->sync_data();
}

void GraphEditorWindow::closeEvent( QCloseEvent* event ) {
    if ( maybeSave() ) {
        writeSettings();
        event->accept();
    }
    else { event->ignore(); }
}

void GraphEditorWindow::newFile() {
    if ( maybeSave() ) {
        m_graph->destroy();
        m_graph_model->sync_data();

        setCurrentFile( "" );
    }
}

void GraphEditorWindow::open() {
    if ( maybeSave() ) {
        QString fileName = QFileDialog::getOpenFileName( this );
        if ( !fileName.isEmpty() ) loadFile( fileName );
    }
}

bool GraphEditorWindow::save() {
    if ( m_curFile.isEmpty() ) { return saveAs(); }
    else { return saveFile( m_curFile ); }
}

bool GraphEditorWindow::saveAs() {
    QFileDialog dialog( this );
    dialog.setWindowModality( Qt::WindowModal );
    dialog.setAcceptMode( QFileDialog::AcceptSave );
    dialog.setDefaultSuffix( "json" );
    if ( dialog.exec() != QDialog::Accepted ) return false;
    return saveFile( dialog.selectedFiles().first() );
}

void GraphEditorWindow::about() {
    QMessageBox::about( this,
                        tr( "About Node Editor" ),
                        tr( "This is NodeGraph Editor widget from Radium::Dataflow::QtGui." ) );
}

void GraphEditorWindow::documentWasModified() {
    setWindowModified( m_graph->shouldBeSaved() );
}

void GraphEditorWindow::createActions() {

    auto fileMenu       = menuBar()->addMenu( tr( "&File" ) );
    auto fileToolBar    = addToolBar( tr( "File" ) );
    const QIcon newIcon = QIcon::fromTheme( "document-new", QIcon( ":/images/new.png" ) );
    auto newAct         = new QAction( newIcon, tr( "&New" ), this );
    newAct->setShortcuts( QKeySequence::New );
    newAct->setStatusTip( tr( "Create a new file" ) );
    connect( newAct, &QAction::triggered, this, &GraphEditorWindow::newFile );
    fileMenu->addAction( newAct );
    fileToolBar->addAction( newAct );

    const QIcon openIcon = QIcon::fromTheme( "document-open", QIcon( ":/images/open.png" ) );
    auto openAct         = new QAction( openIcon, tr( "&Open..." ), this );
    openAct->setShortcuts( QKeySequence::Open );
    openAct->setStatusTip( tr( "Open an existing file" ) );
    connect( openAct, &QAction::triggered, this, &GraphEditorWindow::open );
    fileMenu->addAction( openAct );
    fileToolBar->addAction( openAct );

    const QIcon saveIcon = QIcon::fromTheme( "document-save", QIcon( ":/images/save.png" ) );
    auto saveAct         = new QAction( saveIcon, tr( "&Save" ), this );
    saveAct->setShortcuts( QKeySequence::Save );
    saveAct->setStatusTip( tr( "Save the document to disk" ) );
    connect( saveAct, &QAction::triggered, this, &GraphEditorWindow::save );
    fileMenu->addAction( saveAct );
    fileToolBar->addAction( saveAct );

    const auto saveAsIcon = QIcon::fromTheme( "document-save-as" );
    auto saveAsAct =
        fileMenu->addAction( saveAsIcon, tr( "Save &As..." ), this, &GraphEditorWindow::saveAs );
    saveAsAct->setShortcuts( QKeySequence::SaveAs );
    saveAsAct->setStatusTip( tr( "Save the document under a new name" ) );

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme( "application-exit" );
    auto exitAct         = fileMenu->addAction( exitIcon, tr( "E&xit" ), this, &QWidget::close );
    exitAct->setShortcuts( QKeySequence::Quit );
    exitAct->setStatusTip( tr( "Exit the application" ) );

    auto helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    auto aboutAct = helpMenu->addAction( tr( "&About" ), this, &GraphEditorWindow::about );
    aboutAct->setStatusTip( tr( "Show the application's About box" ) );

    auto aboutQtAct = helpMenu->addAction( tr( "About &Qt" ), qApp, &QApplication::aboutQt );
    aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
    //    if ( !m_ownGraph ) { menuBar()->hide(); }

    viewMenu = menuBar()->addMenu( tr( "&View" ) );
}

void GraphEditorWindow::createStatusBar() {
    statusBar()->showMessage( tr( "Ready" ) );
}

void GraphEditorWindow::readSettings() {
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );
    settings.beginGroup( "nodegraph editor" );
    const QByteArray geometry = settings.value( "geometry", QByteArray() ).toByteArray();
    if ( geometry.isEmpty() ) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize( availableGeometry.width() / 3, availableGeometry.height() / 2 );
        move( ( availableGeometry.width() - width() ) / 2,
              ( availableGeometry.height() - height() ) / 2 );
    }
    else { restoreGeometry( geometry ); }
    settings.endGroup();
}

void GraphEditorWindow::writeSettings() {
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );
    settings.beginGroup( "nodegraph editor" );
    settings.setValue( "geometry", saveGeometry() );
    settings.endGroup();
}

bool GraphEditorWindow::maybeSave() {
    if ( m_graph == nullptr ) { return true; }
    if ( !m_graph->shouldBeSaved() ) { return true; }
    const QMessageBox::StandardButton ret =
        QMessageBox::warning( this,
                              tr( "Application" ),
                              tr( "The document has been modified.\n"
                                  "Do you want to save your changes?" ),
                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
    switch ( ret ) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }

    return true;
}

void GraphEditorWindow::loadFile( const QString& fileName ) {
    QGuiApplication::setOverrideCursor( Qt::WaitCursor );
    {
        QFile file( fileName );
        if ( !file.open( QFile::ReadOnly | QFile::Text ) ) {
            QMessageBox::warning(
                this,
                tr( "Application" ),
                tr( "Cannot read file %1:\n%2." )
                    .arg( QDir::toNativeSeparators( fileName ), file.errorString() ) );
            return;
        }
    }

    bool loaded( true );
    m_graph->destroy();
    loaded = m_graph->loadFromJson( fileName.toStdString() );

    if ( !loaded ) {
        QMessageBox::warning(
            this,
            tr( "Application" ),
            tr( "Can't load graph from file %1.\n" ).arg( QDir::toNativeSeparators( fileName ) ) );
    }

    QGuiApplication::restoreOverrideCursor();
    if ( loaded ) {
        m_graph_model->sync_data();
        m_view->centerScene();
        setCurrentFile( fileName );
        statusBar()->showMessage( tr( "File loaded" ), 2000 );

        emit needUpdate();
    }
}

bool GraphEditorWindow::saveFile( const QString& fileName ) {
    QString errorMessage;

    QGuiApplication::setOverrideCursor( Qt::WaitCursor );
    ///\todo if graph do not compile, tell it to the user ?
    // m_graph->compile();

    m_graph->saveToJson( fileName.toStdString() );

    QGuiApplication::restoreOverrideCursor();

    if ( !errorMessage.isEmpty() ) {
        QMessageBox::warning( this, tr( "Application" ), errorMessage );
        return false;
    }

    setCurrentFile( fileName );
    statusBar()->showMessage( tr( "File saved" ), 2000 );
    return true;
}

void GraphEditorWindow::setCurrentFile( const QString& fileName ) {
    m_curFile = fileName;

    setWindowModified( false );

    QString shownName = m_curFile;
    if ( m_curFile.isEmpty() ) shownName = "untitled.flow";
    setWindowFilePath( shownName );
}

QString GraphEditorWindow::strippedName( const QString& fullFileName ) {
    return QFileInfo( fullFileName ).fileName();
}

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
