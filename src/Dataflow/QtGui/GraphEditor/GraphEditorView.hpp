#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/QtGui/GraphEditor/NodeAdapterModel.hpp>

#include <QWidget>

#include <QtNodes/ConnectionStyle>
#include <QtNodes/DataFlowGraphModel>
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModelRegistry>

namespace Ra {
namespace Dataflow {
namespace Core {
class DataflowGraph;
class NodeFactorySet;
} // namespace Core

/**
 *
 */
namespace QtGui {
namespace GraphEditor {

using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API GraphEditorView : public QWidget
{
    Q_OBJECT
  public:
    explicit GraphEditorView( std::shared_ptr<DataflowGraph> graph, QWidget* parent );
    ~GraphEditorView();

    std::shared_ptr<SimpleGraphModel> getGraph() { return m_graph; }
    void setGraph( std::shared_ptr<DataflowGraph> graph ) {
        m_graph->setGraph( graph );
        sync_data();
    }
    void sync_data() {
        m_graph->sync_data();
        scene->onModelReset();
        view->centerScene();
    }

  private:
    QtNodes::BasicGraphicsScene* scene { nullptr };
    QtNodes::GraphicsView* view { nullptr };

    std::shared_ptr<SimpleGraphModel> m_graph;
};

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
