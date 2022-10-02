#include "MainWindow.hpp"

using namespace Ra::Dataflow::Core;
MainWindow::MainWindow() {
    graphEdit = new GraphEditorView( nullptr );
    setCentralWidget( graphEdit );
    graphEdit->setFocusPolicy( Qt::StrongFocus );

    createActions();
    createStatusBar();

    readSettings();

    connect( graphEdit, &GraphEditorView::needUpdate, this, &MainWindow::documentWasModified );

    setCurrentFile( QString() );
    setUnifiedTitleAndToolBarOnMac( true );
    newFile();
    graphEdit->show();
}

void MainWindow::closeEvent( QCloseEvent* event ) {
    if ( maybeSave() ) {
        writeSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void MainWindow::newFile() {
    if ( maybeSave() ) {
        // Currently edited graph must be deleted only after it is no more used by the editor
        graphEdit->editGraph( nullptr );
        delete graph;

        setCurrentFile( "" );
        graph = new DataflowGraph( "untitled.flow" );
        // auto defaultFactory = NodeFactoriesManager::getDataFlowBuiltInsFactory();
        // graph->addFactory( defaultFactory->getName(), defaultFactory );
        graphEdit->editGraph( graph );
    }
}

void MainWindow::open() {
    if ( maybeSave() ) {
        QString fileName = QFileDialog::getOpenFileName( this );
        if ( !fileName.isEmpty() ) loadFile( fileName );
    }
}

bool MainWindow::save() {
    if ( curFile.isEmpty() ) { return saveAs(); }
    else {
        return saveFile( curFile );
    }
}

bool MainWindow::saveAs() {
    QFileDialog dialog( this );
    dialog.setWindowModality( Qt::WindowModal );
    dialog.setAcceptMode( QFileDialog::AcceptSave );
    if ( dialog.exec() != QDialog::Accepted ) return false;
    return saveFile( dialog.selectedFiles().first() );
}

void MainWindow::about() {
    QMessageBox::about( this,
                        tr( "About Application" ),
                        tr( "The <b>Application</b> example demonstrates how to "
                            "write modern GUI applications using Qt, with a menu bar, "
                            "toolbars, and a status bar." ) );
}

void MainWindow::documentWasModified() {
    setWindowModified( graph->m_recompile );
}

void MainWindow::createActions() {

    QMenu* fileMenu       = menuBar()->addMenu( tr( "&File" ) );
    QToolBar* fileToolBar = addToolBar( tr( "File" ) );
    const QIcon newIcon   = QIcon::fromTheme( "document-new", QIcon( ":/images/new.png" ) );
    QAction* newAct       = new QAction( newIcon, tr( "&New" ), this );
    newAct->setShortcuts( QKeySequence::New );
    newAct->setStatusTip( tr( "Create a new file" ) );
    connect( newAct, &QAction::triggered, this, &MainWindow::newFile );
    fileMenu->addAction( newAct );
    fileToolBar->addAction( newAct );

    const QIcon openIcon = QIcon::fromTheme( "document-open", QIcon( ":/images/open.png" ) );
    QAction* openAct     = new QAction( openIcon, tr( "&Open..." ), this );
    openAct->setShortcuts( QKeySequence::Open );
    openAct->setStatusTip( tr( "Open an existing file" ) );
    connect( openAct, &QAction::triggered, this, &MainWindow::open );
    fileMenu->addAction( openAct );
    fileToolBar->addAction( openAct );

    const QIcon saveIcon = QIcon::fromTheme( "document-save", QIcon( ":/images/save.png" ) );
    QAction* saveAct     = new QAction( saveIcon, tr( "&Save" ), this );
    saveAct->setShortcuts( QKeySequence::Save );
    saveAct->setStatusTip( tr( "Save the document to disk" ) );
    connect( saveAct, &QAction::triggered, this, &MainWindow::save );
    fileMenu->addAction( saveAct );
    fileToolBar->addAction( saveAct );

    const QIcon saveAsIcon = QIcon::fromTheme( "document-save-as" );
    QAction* saveAsAct =
        fileMenu->addAction( saveAsIcon, tr( "Save &As..." ), this, &MainWindow::saveAs );
    saveAsAct->setShortcuts( QKeySequence::SaveAs );
    saveAsAct->setStatusTip( tr( "Save the document under a new name" ) );

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme( "application-exit" );
    QAction* exitAct     = fileMenu->addAction( exitIcon, tr( "E&xit" ), this, &QWidget::close );
    exitAct->setShortcuts( QKeySequence::Quit );
    exitAct->setStatusTip( tr( "Exit the application" ) );

    QMenu* editMenu       = menuBar()->addMenu( tr( "&Edit" ) );
    QToolBar* editToolBar = addToolBar( tr( "Edit" ) );

    QMenu* helpMenu   = menuBar()->addMenu( tr( "&Help" ) );
    QAction* aboutAct = helpMenu->addAction( tr( "&About" ), this, &MainWindow::about );
    aboutAct->setStatusTip( tr( "Show the application's About box" ) );

    QAction* aboutQtAct = helpMenu->addAction( tr( "About &Qt" ), qApp, &QApplication::aboutQt );
    aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage( tr( "Ready" ) );
}

void MainWindow::readSettings() {
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );
    const QByteArray geometry = settings.value( "geometry", QByteArray() ).toByteArray();
    if ( geometry.isEmpty() ) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize( availableGeometry.width() / 3, availableGeometry.height() / 2 );
        move( ( availableGeometry.width() - width() ) / 2,
              ( availableGeometry.height() - height() ) / 2 );
    }
    else {
        restoreGeometry( geometry );
    }
    const QByteArray graphGeometry = settings.value( "graph", QByteArray() ).toByteArray();
    if ( graphGeometry.isEmpty() ) { graphEdit->resize( 800, 600 ); }
    else {
        graphEdit->restoreGeometry( graphGeometry );
    }
}

void MainWindow::writeSettings() {
    QSettings settings( QCoreApplication::organizationName(), QCoreApplication::applicationName() );
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "graph", graphEdit->saveGeometry() );
}

bool MainWindow::maybeSave() {
#if 0
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                                tr("The document has been modified.\n"
                                    "Do you want to save your changes?"),
                                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
#endif
    return true;
}

void MainWindow::loadFile( const QString& fileName )

{
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

    graphEdit->editGraph( nullptr );

    graph = new DataflowGraph( fileName.toStdString() );
    graph->loadFromJson( fileName.toStdString() );

    // Todo, always embed default factory in the graph
    // auto defaultFactory = NodeFactoriesManager::getDataFlowBuiltInsFactory();
    // graph->addFactory( defaultFactory->getName(), defaultFactory );

    graphEdit->editGraph( graph );

    QGuiApplication::restoreOverrideCursor();
    setCurrentFile( fileName );
    statusBar()->showMessage( tr( "File loaded" ), 2000 );
}

bool MainWindow::saveFile( const QString& fileName ) {
    QString errorMessage;

    QGuiApplication::setOverrideCursor( Qt::WaitCursor );
    // TODO, if graph do not compile, tell it to the user ?
    graph->compile();
    graph->saveToJson( fileName.toStdString() );
#if 0
    QSaveFile file(fileName);

    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
#endif
    QGuiApplication::restoreOverrideCursor();

    if ( !errorMessage.isEmpty() ) {
        QMessageBox::warning( this, tr( "Application" ), errorMessage );
        return false;
    }

    setCurrentFile( fileName );
    statusBar()->showMessage( tr( "File saved" ), 2000 );
    return true;
}

void MainWindow::setCurrentFile( const QString& fileName ) {
    curFile = fileName;
    // textEdit->document()->setModified(false);
    setWindowModified( false );

    QString shownName = curFile;
    if ( curFile.isEmpty() ) shownName = "untitled.flow";
    setWindowFilePath( shownName );
}

QString MainWindow::strippedName( const QString& fullFileName ) {
    return QFileInfo( fullFileName ).fileName();
}
