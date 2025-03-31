#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>
#include <Dataflow/Core/Node.hpp>

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

/**
 * Adapter between DataflowGraph and QtNodes, bare minimal, based on simple_graph QtNodes example.
 */
class GraphModel : public QtNodes::AbstractGraphModel
{
    using ConnectionId     = QtNodes::ConnectionId;
    using ConnectionPolicy = QtNodes::ConnectionPolicy;
    using NodeFlag         = QtNodes::NodeFlag;
    using NodeId           = QtNodes::NodeId;
    using NodeRole         = QtNodes::NodeRole;
    using PortIndex        = QtNodes::PortIndex;
    using PortRole         = QtNodes::PortRole;
    using PortType         = QtNodes::PortType;
    using StyleCollection  = QtNodes::StyleCollection;

    Q_OBJECT
  public:
    struct NodeGeometryData {
        QSize size;
        QPointF pos;
    };

  public:
    GraphModel( std::shared_ptr<Core::DataflowGraph> graph );

    ~GraphModel() override;

    std::unordered_set<NodeId> allNodeIds() const override;

    std::unordered_set<ConnectionId> allConnectionIds( NodeId const nodeId ) const override;

    std::unordered_set<ConnectionId>
    connections( NodeId nodeId, PortType portType, PortIndex portIndex ) const override;

    bool connectionExists( ConnectionId const connectionId ) const override;

    void addInputOutputNodesForGraph();
    NodeId addNode( QString const nodeType = QString() ) override;

    /**
     * Connection is possible when graph contains no connectivity data
     * in both directions `Out -> In` and `In -> Out`.
     */
    bool connectionPossible( ConnectionId const connectionId ) const override;

    void addConnection( ConnectionId const connectionId ) override;

    bool nodeExists( NodeId const nodeId ) const override;

    QWidget* getWidget( std::shared_ptr<Core::Node> node ) const;
    QVariant nodeData( NodeId nodeId, NodeRole role ) const override;

    bool setNodeData( NodeId nodeId, NodeRole role, QVariant value ) override;

    QVariant
    portData( NodeId nodeId, PortType portType, PortIndex portIndex, PortRole role ) const override;

    bool setPortData( NodeId nodeId,
                      PortType portType,
                      PortIndex portIndex,
                      QVariant const& value,
                      PortRole role = PortRole::Data ) override;

    bool deleteConnection( ConnectionId const connectionId ) override;

    bool deleteNode( NodeId const nodeId ) override;

    NodeId newNodeId() override { return _nextNodeId++; }
    /// needed for undo/redo
    void loadNode( QJsonObject const& nodeJson ) override;
    QJsonObject saveNode( NodeId const ) const override;

    void setGraph( std::shared_ptr<Core::DataflowGraph> graph );
    void sync_data();
    void clear_node_widget( Core::Node* node );

    auto node_ptr( NodeId node_id ) -> std::shared_ptr<Core::Node> {
        return m_node_id_to_ptr.at( node_id );
    }

  signals:
    void node_edited( std::shared_ptr<Core::Node> node );

  private:
    std::shared_ptr<Core::DataflowGraph> m_graph;
    std::unordered_set<NodeId> m_node_ids;
    std::map<NodeId, std::shared_ptr<Core::Node>> m_node_id_to_ptr;

    /// This data structure contains the graph connectivity information in both
    /// directions, i.e. from Node1 to Node2 and from Node2 to Node1.
    std::unordered_set<ConnectionId> m_connectivity;

    mutable std::unordered_map<NodeId, NodeGeometryData> m_node_geometry_data;
    mutable std::unordered_map<NodeId, QWidget*> m_node_widget;

    /// A convenience variable needed for generating unique node ids.
    NodeId _nextNodeId;

    void buildFactoryMap();
    std::map<std::string, Core::NodeFactory::NodeCreatorFunctor> m_model_name_to_factory;
};
} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
