#pragma once
#include <QMainWindow>
#include <QtWidgets>

#include <Dataflow/QtGui/GraphEditor/GraphEditorView.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

using namespace Ra::Dataflow::QtGui::GraphEditor;

/// TODO : transform this example in a grapheWidget allowing to edit graph or subgraphs
class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow();

    void loadFile( const QString& fileName );

  protected:
    void closeEvent( QCloseEvent* event ) override;

  private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();

  private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile( const QString& fileName );
    void setCurrentFile( const QString& fileName );
    QString strippedName( const QString& fullFileName );

    GraphEditorView* graphEdit;
    QString curFile;

    DataflowGraph* graph { nullptr };
};
