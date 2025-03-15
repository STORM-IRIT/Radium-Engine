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

    dock->setWidget( node_tree_widget );
    addDockWidget( Qt::LeftDockWidgetArea, dock );
    viewMenu->addAction( dock->toggleViewAction() );
}

void GraphEditorWindow::addNode( QTreeWidgetItem* item, int ) {
    if ( item->childCount() == 0 ) { m_graph_model->addNode( item->text( 0 ) ); }
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
        m_view->centerScene();

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
