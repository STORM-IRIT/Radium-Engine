#include <Dataflow/QtGui/GraphEditor/GraphModel.hpp>

#include <Dataflow/QtGui/GraphEditor/WidgetFactory.hpp>
#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>
#include <QPushButton>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {
using namespace Ra::Dataflow::Core;

GraphModel::GraphModel( std::shared_ptr<Core::DataflowGraph> graph ) :
    m_graph { graph }, m_next_node_id { 0 } {
    fill_factory_map();
    sync_data();
}

GraphModel::~GraphModel() {
    //
}

void GraphModel::fill_factory_map() {
    auto factories = NodeFactoriesManager::getFactoryManager();
    for ( const auto& [factoryName, factory] : factories ) {
        for ( const auto& [model_name, creator] : factory->getFactoryMap() ) {
            auto f                              = creator.first;
            m_model_name_to_factory[model_name] = f;
        }
    }
}

std::unordered_set<GraphModel::NodeId> GraphModel::allNodeIds() const {
    return m_node_ids;
}

std::unordered_set<GraphModel::ConnectionId>
GraphModel::allConnectionIds( NodeId const nodeId ) const {
    std::unordered_set<ConnectionId> result;

    std::copy_if( m_connectivity.begin(),
                  m_connectivity.end(),
                  std::inserter( result, std::end( result ) ),
                  [&nodeId]( ConnectionId const& cid ) {
                      return cid.inNodeId == nodeId || cid.outNodeId == nodeId;
                  } );

    return result;
}

std::unordered_set<GraphModel::ConnectionId>
GraphModel::connections( NodeId nodeId, PortType portType, PortIndex portIndex ) const {
    std::unordered_set<ConnectionId> result;

    std::copy_if( m_connectivity.begin(),
                  m_connectivity.end(),
                  std::inserter( result, std::end( result ) ),
                  [&portType, &portIndex, &nodeId]( ConnectionId const& cid ) {
                      return ( getNodeId( portType, cid ) == nodeId &&
                               getPortIndex( portType, cid ) == portIndex );
                  } );

    return result;
}

bool GraphModel::connectionExists( ConnectionId const connectionId ) const {
    return ( m_connectivity.find( connectionId ) != m_connectivity.end() );
}

void GraphModel::addInputOutputNodesForGraph() {
    m_graph->add_input_output_nodes();
    sync_data();
}

GraphModel::NodeId GraphModel::addNode( QString const nodeType ) {
    if ( nodeType.toStdString() == GraphInputNode::getTypename() ||
         nodeType.toStdString() == GraphOutputNode::getTypename() ) {
        m_graph->add_input_output_nodes();
        sync_data();
        auto itr = std::find_if( m_node_id_to_ptr.begin(),
                                 m_node_id_to_ptr.end(),
                                 [node_typename = nodeType.toStdString()]( const auto& n ) {
                                     return n.second->model_name() == node_typename;
                                 } );
        return itr->first;
    }
    auto f = m_model_name_to_factory.at( nodeType.toStdString() );
    auto n = f( {} );
    m_graph->addNode( n );

    NodeId newId = newNodeId();
    m_node_ids.insert( newId );
    m_node_id_to_ptr[newId] = n;

    Q_EMIT nodeCreated( newId );

    return newId;
}

bool GraphModel::connectionPossible( ConnectionId const connectionId ) const {
    auto in_node_id  = connectionId.inNodeId;
    auto out_node_id = connectionId.outNodeId;

    auto in_port_id  = connectionId.inPortIndex;
    auto out_port_id = connectionId.outPortIndex;

    bool ret = m_connectivity.find( connectionId ) == m_connectivity.end() &&
               m_graph->canLink( m_node_id_to_ptr.at( out_node_id ).get(),
                                 out_port_id,
                                 m_node_id_to_ptr.at( in_node_id ).get(),
                                 in_port_id );

    return ret;
}

void GraphModel::addConnection( ConnectionId const connectionId ) {
    m_connectivity.insert( connectionId );
    auto in_node_id  = connectionId.inNodeId;
    auto out_node_id = connectionId.outNodeId;
    auto in_port_id  = connectionId.inPortIndex;
    auto out_port_id = connectionId.outPortIndex;

    m_graph->addLink( m_node_id_to_ptr.at( out_node_id ),
                      out_port_id,
                      m_node_id_to_ptr.at( in_node_id ),
                      in_port_id );

    Q_EMIT connectionCreated( connectionId );
    Q_EMIT nodeUpdated( in_node_id );
    Q_EMIT nodeUpdated( out_node_id );
}

bool GraphModel::nodeExists( NodeId const nodeId ) const {
    return ( m_node_ids.find( nodeId ) != m_node_ids.end() );
}

QWidget* GraphModel::getWidget( std::shared_ptr<Core::Node> node ) const {
    QWidget* controlPanel = new QWidget;
    controlPanel->setStyleSheet( "background-color:transparent;" );
    QVBoxLayout* layout = new QVBoxLayout( controlPanel );

    auto node_inputs = node->input_variables();
    if ( node_inputs.size() > 0 ) {
        auto controlPanelInputs = new Ra::Gui::VariableSetEditor( "Inputs default", nullptr );
        controlPanelInputs->setShowUnspecified( true );

        WidgetFactory ui_builder { node_inputs, controlPanelInputs, {} };
        node_inputs.visit( ui_builder );
        // empty controlPanel has 3 children, if no more, then no widget for the inputs
        if ( controlPanelInputs->children().size() > 3 )
            layout->addWidget( controlPanelInputs );
        else
            delete controlPanelInputs;
    }
    if ( node->parameters().size() > 0 ) {
        auto controlPanelParams = new Ra::Gui::VariableSetEditor( "Parameters", nullptr );
        controlPanelParams->setShowUnspecified( true );

        WidgetFactory ui_builder { node->parameters(), controlPanelParams, {} };
        node_inputs.visit( ui_builder );

        layout->addWidget( controlPanelParams );
    }
    auto g = dynamic_cast<DataflowGraph*>( node.get() );
    if ( g ) {
        auto b = new QPushButton( "Edit" );
        b->setFlat( true );
        connect( b, &QPushButton::clicked, [this, node]() {
            emit const_cast<GraphModel*>( this )->node_edited( node );
        } );
        layout->addWidget( b );
    }
    return controlPanel;
}

QVariant GraphModel::nodeData( NodeId nodeId, NodeRole role ) const {

    QVariant result;
    auto node_ptr = m_node_id_to_ptr.at( nodeId );

    switch ( role ) {
    case NodeRole::Type:
        result = QString::fromStdString( node_ptr->model_name() );
        break;

    case NodeRole::Position:
        result = m_node_geometry_data[nodeId].pos;
        break;

    case NodeRole::Size:
        result = m_node_geometry_data[nodeId].size;
        break;

    case NodeRole::CaptionVisible:
        result = true;
        break;

    case NodeRole::Caption:
        result = QString::fromStdString( node_ptr->display_name() );
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result     = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount: {
        unsigned int count = ( node_ptr->inputs().size() );
        if ( node_ptr == m_graph->output_node() ) ++count;
        result = ( count );
    } break;

    case NodeRole::OutPortCount: {
        unsigned int count = ( node_ptr->outputs().size() );
        if ( node_ptr == m_graph->input_node() ) ++count;
        result = ( count );
    } break;

    case NodeRole::Widget:
        if ( auto node_itr = m_node_widget.find( nodeId ); node_itr == m_node_widget.end() ) {
            m_node_widget[nodeId] = getWidget( node_ptr );
        }
        result = QVariant::fromValue( m_node_widget[nodeId] );
        break;
    }

    return result;
}

bool GraphModel::setNodeData( NodeId nodeId, NodeRole role, QVariant value ) {
    bool result   = false;
    auto node_ptr = m_node_id_to_ptr.at( nodeId );

    switch ( role ) {
    case NodeRole::Type:
        break;
    case NodeRole::Position: {
        auto pos = value.value<QPointF>();

        m_node_geometry_data[nodeId].pos = pos;
        nlohmann::json json = { { "position", { { "x", pos.x() }, { "y", pos.y() } } } };
        node_ptr->add_metadata( json );
        emit nodePositionUpdated( nodeId );

        result = true;
    } break;

    case NodeRole::Size: {
        m_node_geometry_data[nodeId].size = value.value<QSize>();
        result                            = true;
    } break;

    case NodeRole::CaptionVisible:
        break;

    case NodeRole::Caption:
        break;

    case NodeRole::Style:
        break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        break;

    case NodeRole::OutPortCount:
        break;

    case NodeRole::Widget:
        break;
    }

    return result;
}

QVariant
GraphModel::portData( NodeId nodeId, PortType portType, PortIndex portIndex, PortRole role ) const {

    auto n = m_node_id_to_ptr.at( nodeId );
    if ( n == m_graph->input_node() || n == m_graph->output_node() ) {
        switch ( role ) {
        case PortRole::Data:
            return QVariant();
            break;

        case PortRole::DataType: {
            QString s;
            if ( ( n == m_graph->input_node() && portIndex == n->inputs().size() ) ||
                 ( n == m_graph->output_node() && portIndex == n->inputs().size() ) )
                s = QString( "any" );
            else {
                auto p = ( portType == PortType::In ) ? n->port_by_index( "in", portIndex )
                                                      : n->port_by_index( "out", portIndex );

                s = QString::fromStdString(
                    Ra::Core::Utils::simplifiedDemangledType( p->getType() ) );
            }
            return QVariant::fromValue( QtNodes::NodeDataType { s, s } );
        } break;

        case PortRole::ConnectionPolicyRole:
            if ( portType == PortType::In )
                return QVariant::fromValue( ConnectionPolicy::One );
            else
                return QVariant::fromValue( ConnectionPolicy::Many );
            break;

        case PortRole::CaptionVisible:
            return true;
            break;

        case PortRole::Caption: {
            if ( n == m_graph->input_node() && portIndex == n->inputs().size() )
                return QString( "new" );
            if ( n == m_graph->output_node() && portIndex == n->inputs().size() )
                return QString( "new" );

            auto p = ( portType == PortType::In ) ? n->port_by_index( "in", portIndex )
                                                  : n->port_by_index( "out", portIndex );
            return QString::fromStdString( p->getName() );
        } break;
        }
        return QVariant();
    }

    switch ( role ) {
    case PortRole::Data:
        return QVariant();
        break;

    case PortRole::DataType: {
        auto p = ( portType == PortType::In ) ? n->port_by_index( "in", portIndex )
                                              : n->port_by_index( "out", portIndex );
        QString s =
            QString::fromStdString( Ra::Core::Utils::simplifiedDemangledType( p->getType() ) );
        return QVariant::fromValue( QtNodes::NodeDataType { s, s } );
    } break;

    case PortRole::ConnectionPolicyRole:
        if ( portType == PortType::In )
            return QVariant::fromValue( ConnectionPolicy::One );
        else
            return QVariant::fromValue( ConnectionPolicy::Many );
        break;

    case PortRole::CaptionVisible:
        return true;
        break;

    case PortRole::Caption: {
        auto p = ( portType == PortType::In ) ? n->port_by_index( "in", portIndex )
                                              : n->port_by_index( "out", portIndex );
        return QString::fromStdString( p->getName() );
    } break;
    }

    return QVariant();
}

bool GraphModel::setPortData( NodeId nodeId,
                              PortType portType,
                              PortIndex portIndex,
                              QVariant const& value,
                              PortRole role ) {
    Q_UNUSED( nodeId );
    Q_UNUSED( portType );
    Q_UNUSED( portIndex );
    Q_UNUSED( value );
    Q_UNUSED( role );

    return false;
}

bool GraphModel::deleteConnection( ConnectionId const connectionId ) {
    bool disconnected = false;

    auto it = m_connectivity.find( connectionId );

    if ( it != m_connectivity.end() ) {
        disconnected    = true;
        auto in_node_id = connectionId.inNodeId;
        auto in_port_id = connectionId.inPortIndex;

        m_graph->removeLink( m_node_id_to_ptr.at( in_node_id ), in_port_id );
        m_connectivity.erase( it );
    }

    if ( disconnected ) emit connectionDeleted( connectionId );

    return disconnected;
}

bool GraphModel::deleteNode( NodeId const nodeId ) {
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds( nodeId );

    for ( auto& cId : connectionIds ) {
        deleteConnection( cId );
    }

    m_graph->removeNode( m_node_id_to_ptr.at( nodeId ) );
    m_node_id_to_ptr.erase( nodeId );
    m_node_widget.erase( nodeId );

    m_node_ids.erase( nodeId );
    m_node_geometry_data.erase( nodeId );

    emit nodeDeleted( nodeId );

    return true;
}

QJsonObject GraphModel::saveNode( NodeId const nodeId ) const {
    QJsonObject nodeJson;

    auto node = m_node_id_to_ptr.at( nodeId );

    // get node's json
    nlohmann::json json;
    node->toJson( json );
    {
        // appens ui stuff
        json["id"]        = static_cast<qint64>( nodeId );
        QPointF const pos = nodeData( nodeId, NodeRole::Position ).value<QPointF>();
        json["position"]  = { { "x", pos.x() }, { "y", pos.y() } };
    }

    // convert to QJsonObject
    QJsonDocument jsonResponse = QJsonDocument::fromJson( json.dump().c_str() );
    QJsonObject jsonObject     = jsonResponse.object();
    for ( auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); it++ ) {
        nodeJson.insert( it.key(), it.value() );
    }

    return nodeJson;
}

void GraphModel::loadNode( QJsonObject const& nodeJson ) {

    // init node from json
    auto json = nlohmann::json::parse( QJsonDocument( nodeJson ).toJson() );
    auto f    = m_model_name_to_factory[json["model"]["name"]];
    auto n    = f( json );
    m_graph->addNode( n );

    // restore model and ui stuff
    NodeId restoredNodeId = static_cast<NodeId>( nodeJson["id"].toInt() );

    // Next NodeId must be larger that any id existing in the graph
    m_next_node_id = std::max( m_next_node_id, restoredNodeId + 1 );

    // Create new node.
    m_node_ids.insert( restoredNodeId );

    // Create new node.
    m_node_ids.insert( restoredNodeId );
    m_node_id_to_ptr[restoredNodeId] = n;

    Q_EMIT nodeCreated( restoredNodeId );

    {
        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos( posJson["x"].toDouble(), posJson["y"].toDouble() );

        setNodeData( restoredNodeId, NodeRole::Position, pos );
    }
}

void GraphModel::setGraph( std::shared_ptr<Core::DataflowGraph> graph ) {
    m_graph = graph;
    sync_data();
}

void GraphModel::sync_data() {
    m_node_ids.clear();
    m_node_id_to_ptr.clear();
    m_connectivity.clear();
    m_node_geometry_data.clear();
    m_node_widget.clear();
    m_next_node_id = 0;

    // Create new nodes
    for ( const auto& n : m_graph->getNodes() ) {
        NodeId newId = newNodeId();
        m_node_ids.insert( newId );
        m_node_id_to_ptr[newId] = n;
        if ( auto position = n->metadata().find( "position" ); position != n->metadata().end() ) {
            m_node_geometry_data[newId].pos.setX( position->at( "x" ) );
            m_node_geometry_data[newId].pos.setY( position->at( "y" ) );
        }
    }

    // from nodes input to output
    for ( const auto& in_node : m_graph->getNodes() ) {
        // get in_node_id, skip m_graph input_node
        auto in_node_itr = std::find_if(
            m_node_id_to_ptr.begin(), m_node_id_to_ptr.end(), [in_node]( const auto& pair ) {
                return pair.second.get() == in_node.get();
            } );
        if ( in_node_itr == m_node_id_to_ptr.end() ) {
            LOG( Ra::Core::Utils::logERROR ) << "error graph structure in_node";
            return;
        }

        // skip connection outside graph
        if ( in_node_itr->second == m_graph->input_node() ) { continue; }

        const auto& in_node_id = in_node_itr->first;

        for ( size_t in_port_id = 0; in_port_id < in_node->inputs().size(); ++in_port_id ) {
            const auto& in_port  = in_node->inputs()[in_port_id];
            const auto& out_port = in_port->getLink();

            if ( out_port ) {
                // get out node id
                auto out_node = out_port->getNode();
                if ( out_node ) {
                    // if linked to graph out_node, not from the model's graph, use
                    // out_node->graph as out_node
                    const auto graph_out_node = dynamic_cast<const GraphOutputNode*>( out_node );
                    if ( graph_out_node && graph_out_node->graph() != m_graph.get() ) {
                        out_node = graph_out_node->graph();
                    }

                    auto out_node_itr = std::find_if(
                        m_node_id_to_ptr.begin(),
                        m_node_id_to_ptr.end(),
                        [out_node]( const auto& pair ) { return pair.second.get() == out_node; } );
                    if ( out_node_itr == m_node_id_to_ptr.end() ) {
                        LOG( Ra::Core::Utils::logERROR )
                            << "error graph structure out_node, port " << out_port->getName()
                            << " in node " << in_node->display_name() << " " << in_port->getName();
                        return;
                    }

                    const auto& out_node_id = out_node_itr->first;

                    // get out port id
                    auto out_port_itr =
                        find_if( out_node->outputs().begin(),
                                 out_node->outputs().end(),
                                 [out_port]( auto p ) { return p.get() == out_port; } );
                    if ( out_port_itr == out_node->outputs().end() ) {
                        LOG( Ra::Core::Utils::logERROR )
                            << "error graph structure, out node " << out_node->display_name()
                            << " out_port, in node " << in_node->display_name() << " "
                            << in_port->getName();
                        return;
                    }
                    const auto out_port_id =
                        std::distance( out_node->outputs().begin(), out_port_itr );

                    // set connection
                    ConnectionId connection_id;

                    connection_id.inNodeId     = in_node_id;
                    connection_id.outNodeId    = out_node_id;
                    connection_id.inPortIndex  = in_port_id;
                    connection_id.outPortIndex = out_port_id;

                    m_connectivity.insert( connection_id );
                }
            }
        }
    }
    emit modelReset();
}

void GraphModel::clear_node_widget( Core::Node* node ) {
    auto node_itr =
        std::find_if( m_node_id_to_ptr.begin(), m_node_id_to_ptr.end(), [node]( const auto& pair ) {
            return pair.second.get() == node;
        } );
    if ( node_itr == m_node_id_to_ptr.end() ) {
        LOG( Ra::Core::Utils::logERROR ) << "error try to clear widget of an unmanaged node";
        return;
    }
    const auto& node_id = node_itr->first;
    m_node_widget.erase( node_id );
}

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
