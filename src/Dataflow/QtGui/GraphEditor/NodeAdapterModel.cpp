#include <Dataflow/QtGui/GraphEditor/NodeAdapterModel.hpp>

#include <Gui/ParameterSetEditor/ParameterSetEditor.hpp>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {
using namespace Ra::Dataflow::Core;
using ConnectionId     = QtNodes::ConnectionId;
using ConnectionPolicy = QtNodes::ConnectionPolicy;
using NodeFlag         = QtNodes::NodeFlag;
using NodeId           = QtNodes::NodeId;
using NodeRole         = QtNodes::NodeRole;
using PortIndex        = QtNodes::PortIndex;
using PortRole         = QtNodes::PortRole;
using PortType         = QtNodes::PortType;
using StyleCollection  = QtNodes::StyleCollection;
using QtNodes::InvalidNodeId;

SimpleGraphModel::SimpleGraphModel() : _nextNodeId { 0 } {
    m_graph = std::make_shared<DataflowGraph>( "graph" );
    buildFactoryMap();
}

SimpleGraphModel::~SimpleGraphModel() {
    //
}

void SimpleGraphModel::buildFactoryMap() {
    auto factories = NodeFactoriesManager::getFactoryManager();
    for ( const auto& [factoryName, factory] : factories ) {
        for ( const auto& [model_name, creator] : factory->getFactoryMap() ) {
            auto f                              = creator.first;
            auto creatorFactory                 = factory;
            m_model_name_to_factory[model_name] = f;
        }
    }
}

std::unordered_set<NodeId> SimpleGraphModel::allNodeIds() const {
    return _nodeIds;
}

std::unordered_set<ConnectionId> SimpleGraphModel::allConnectionIds( NodeId const nodeId ) const {
    std::unordered_set<ConnectionId> result;

    std::copy_if( _connectivity.begin(),
                  _connectivity.end(),
                  std::inserter( result, std::end( result ) ),
                  [&nodeId]( ConnectionId const& cid ) {
                      return cid.inNodeId == nodeId || cid.outNodeId == nodeId;
                  } );

    return result;
}

std::unordered_set<ConnectionId>
SimpleGraphModel::connections( NodeId nodeId, PortType portType, PortIndex portIndex ) const {
    std::unordered_set<ConnectionId> result;

    std::copy_if( _connectivity.begin(),
                  _connectivity.end(),
                  std::inserter( result, std::end( result ) ),
                  [&portType, &portIndex, &nodeId]( ConnectionId const& cid ) {
                      return ( getNodeId( portType, cid ) == nodeId &&
                               getPortIndex( portType, cid ) == portIndex );
                  } );

    return result;
}

bool SimpleGraphModel::connectionExists( ConnectionId const connectionId ) const {
    return ( _connectivity.find( connectionId ) != _connectivity.end() );
}

NodeId SimpleGraphModel::addNode( QString const nodeType ) {

    auto f = m_model_name_to_factory[nodeType.toStdString()];
    auto n = f( {} );
    m_graph->addNode( n );

    NodeId newId = newNodeId();
    // Create new node.
    _nodeIds.insert( newId );
    m_node_id_to_ptr[newId] = n;

    Q_EMIT nodeCreated( newId );

    return newId;
}

bool SimpleGraphModel::connectionPossible( ConnectionId const connectionId ) const {
    auto in_node_id  = connectionId.inNodeId;
    auto out_node_id = connectionId.outNodeId;

    auto in_port_id  = connectionId.inPortIndex;
    auto out_port_id = connectionId.outPortIndex;

    bool ret = _connectivity.find( connectionId ) == _connectivity.end() &&
               m_graph->canLink( m_node_id_to_ptr.at( out_node_id ).get(),
                                 out_port_id,
                                 m_node_id_to_ptr.at( in_node_id ).get(),
                                 in_port_id );

    return ret;
}

void SimpleGraphModel::addConnection( ConnectionId const connectionId ) {
    _connectivity.insert( connectionId );
    auto in_node_id  = connectionId.inNodeId;
    auto out_node_id = connectionId.outNodeId;
    auto in_port_id  = connectionId.inPortIndex;
    auto out_port_id = connectionId.outPortIndex;

    m_graph->addLink( m_node_id_to_ptr.at( out_node_id ),
                      out_port_id,
                      m_node_id_to_ptr.at( in_node_id ),
                      in_port_id );
    Q_EMIT connectionCreated( connectionId );
}

bool SimpleGraphModel::nodeExists( NodeId const nodeId ) const {
    return ( _nodeIds.find( nodeId ) != _nodeIds.end() );
}

QWidget* SimpleGraphModel::getWidget( std::shared_ptr<Core::Node> node ) const {
    QWidget* controlPanel = new QWidget;
    controlPanel->setStyleSheet( "background-color:transparent;" );
    QVBoxLayout* layout = new QVBoxLayout( controlPanel );

    if ( node->getInputVariables().size() > 0 ) {
        auto controlPanelInputs = new Ra::Gui::VariableSetEditor( "Inputs default", nullptr );
        controlPanelInputs->setShowUnspecified( true );
        controlPanelInputs->setupUi( node->getInputVariables(), {} );
        layout->addWidget( controlPanelInputs );
    }
    if ( node->getParameters().size() > 0 ) {
        auto controlPanelParams = new Ra::Gui::VariableSetEditor( "Parameters", nullptr );
        controlPanelParams->setShowUnspecified( true );
        controlPanelParams->setupUi( node->getParameters(), {} );
        layout->addWidget( controlPanelParams );
    }
    return controlPanel;
}

QVariant SimpleGraphModel::nodeData( NodeId nodeId, NodeRole role ) const {

    QVariant result;
    auto node_ptr = m_node_id_to_ptr.at( nodeId );

    switch ( role ) {
    case NodeRole::Type:
        result = QString::fromStdString( node_ptr->getTypename() );
        break;

    case NodeRole::Position:
        result = _nodeGeometryData[nodeId].pos;
        break;

    case NodeRole::Size:
        result = _nodeGeometryData[nodeId].size;
        break;

    case NodeRole::CaptionVisible:
        result = true;
        break;

    case NodeRole::Caption:
        result = QString::fromStdString( node_ptr->getModelName() );
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result     = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        result = static_cast<unsigned int>( node_ptr->getInputs().size() );
        break;

    case NodeRole::OutPortCount:
        result = static_cast<unsigned int>( node_ptr->getOutputs().size() );
        break;

    case NodeRole::Widget:

        if ( auto node_itr = m_node_widget.find( nodeId ); node_itr == m_node_widget.end() ) {
            m_node_widget[nodeId] = getWidget( node_ptr );
        }
        result = QVariant::fromValue( m_node_widget[nodeId] );
        break;
    }

    return result;
}

bool SimpleGraphModel::setNodeData( NodeId nodeId, NodeRole role, QVariant value ) {
    bool result   = false;
    auto node_ptr = m_node_id_to_ptr.at( nodeId );

    switch ( role ) {
    case NodeRole::Type:
        break;
    case NodeRole::Position: {
        auto pos = value.value<QPointF>();

        _nodeGeometryData[nodeId].pos = pos;

        nlohmann::json json = { { "position", { "x", pos.x() }, { "y", pos.y() } } };

        node_ptr->addJsonMetaData( json );
        emit nodePositionUpdated( nodeId );

        result = true;
    } break;

    case NodeRole::Size: {
        _nodeGeometryData[nodeId].size = value.value<QSize>();
        result                         = true;
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

QVariant SimpleGraphModel::portData( NodeId nodeId,
                                     PortType portType,
                                     PortIndex portIndex,
                                     PortRole role ) const {

    auto n = m_node_id_to_ptr.at( nodeId );

    switch ( role ) {
    case PortRole::Data:
        return QVariant();
        break;

    case PortRole::DataType: {
        auto p = ( portType == PortType::In ) ? n->getPortByIndex( "in", portIndex )
                                              : n->getPortByIndex( "out", portIndex );
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
        auto p = ( portType == PortType::In ) ? n->getPortByIndex( "in", portIndex )
                                              : n->getPortByIndex( "out", portIndex );
        return QString::fromStdString( p->getName() );
    } break;
    }

    return QVariant();
}

bool SimpleGraphModel::setPortData( NodeId nodeId,
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

bool SimpleGraphModel::deleteConnection( ConnectionId const connectionId ) {
    bool disconnected = false;

    auto it = _connectivity.find( connectionId );

    if ( it != _connectivity.end() ) {
        disconnected    = true;
        auto in_node_id = connectionId.inNodeId;
        auto in_port_id = connectionId.inPortIndex;

        m_graph->removeLink( m_node_id_to_ptr.at( in_node_id ), in_port_id );
        _connectivity.erase( it );
    }

    if ( disconnected ) emit connectionDeleted( connectionId );

    return disconnected;
}

bool SimpleGraphModel::deleteNode( NodeId const nodeId ) {
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds( nodeId );

    for ( auto& cId : connectionIds ) {
        deleteConnection( cId );
    }

    m_graph->removeNode( m_node_id_to_ptr.at( nodeId ) );
    m_node_id_to_ptr.erase( nodeId );
    m_node_widget.erase( nodeId );

    _nodeIds.erase( nodeId );
    _nodeGeometryData.erase( nodeId );

    emit nodeDeleted( nodeId );

    return true;
}

QJsonObject SimpleGraphModel::saveNode( NodeId const nodeId ) const {
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

void SimpleGraphModel::loadNode( QJsonObject const& nodeJson ) {

    // init node from json
    auto json = nlohmann::json::parse( QJsonDocument( nodeJson ).toJson() );
    auto f    = m_model_name_to_factory[json["model"]["name"]];
    auto n    = f( json );
    m_graph->addNode( n );

    // restore model and ui stuff
    NodeId restoredNodeId = static_cast<NodeId>( nodeJson["id"].toInt() );

    // Next NodeId must be larger that any id existing in the graph
    _nextNodeId = std::max( _nextNodeId, restoredNodeId + 1 );

    // Create new node.
    _nodeIds.insert( restoredNodeId );

    // Create new node.
    _nodeIds.insert( restoredNodeId );
    m_node_id_to_ptr[restoredNodeId] = n;

    Q_EMIT nodeCreated( restoredNodeId );

    {
        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos( posJson["x"].toDouble(), posJson["y"].toDouble() );

        setNodeData( restoredNodeId, NodeRole::Position, pos );
    }
}

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
