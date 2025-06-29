#pragma once
#include <Dataflow/RaDataflow.hpp>

// include first for qt/gl include order
#include <Gui/RaGui.hpp>

#include <QMainWindow>
#include <QtWidgets>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/QtGui/GraphEditor/GraphModel.hpp>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {

using namespace Ra::Dataflow::Core;

/**
 * \brief Window widget to edit a DataflowGraph.
 * This class wraps  DataflowGraph, GraphModel, QtNodes::BasicGraphicsScene and
 * QtNodes::GraphicsView in a main window with
 * several services :
 *   - The window can be used as a standalone editor if no parameter is given to the constructor.
 *   - The window can be used to edit an existing graph by giving the graph to the constructor.
 *
 *   connect the client to the needUpdate() signal to be notified of a change in the edited graph.
 */
class RA_DATAFLOW_GUI_API GraphEditorWindow : public QMainWindow
{
    Q_OBJECT
  public:
    explicit GraphEditorWindow( std::shared_ptr<DataflowGraph> graph = nullptr );
    ~GraphEditorWindow();

    void loadFile( const QString& fileName );
    std::shared_ptr<GraphModel> graph_model() { return m_graph_model; }

  signals:
    void needUpdate();

  protected:
    void closeEvent( QCloseEvent* event ) override;

  private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void add_node( QTreeWidgetItem* item, int column );
    void node_editor( std::shared_ptr<Node> node );
    void node_dialog( QtNodes::NodeId node_id );

  private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile( const QString& fileName );
    void setCurrentFile( const QString& fileName );
    QString strippedName( const QString& fullFileName );

    std::shared_ptr<DataflowGraph> m_graph { nullptr };
    std::shared_ptr<GraphModel> m_graph_model { nullptr };

    QtNodes::BasicGraphicsScene* m_scene { nullptr };
    QtNodes::GraphicsView* m_view { nullptr };

    QString m_curFile { "" };

    QMenu* viewMenu { nullptr };
};

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
