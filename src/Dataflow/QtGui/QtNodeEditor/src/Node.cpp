#include "Node.hpp"

#include <QtCore/QObject>

#include <iostream>
#include <utility>

#include "FlowScene.hpp"

#include "NodeDataModel.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

using QtNodes::Node;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeState;
using QtNodes::PortIndex;
using QtNodes::PortType;

// TODO MTHS -- get uuid from datamodel instead of generating one
Node::Node( std::unique_ptr<NodeDataModel>&& dataModel ) :
    _uid( QUuid::createUuid() ),
    _nodeDataModel( std::move( dataModel ) ),
    _nodeState( _nodeDataModel ),
    _nodeGeometry( _nodeDataModel ),
    _nodeGraphicsObject( nullptr ) {
    _nodeGeometry.recalculateSize();

    // propagate data: model => node
    connect( _nodeDataModel.get(), &NodeDataModel::dataUpdated, this, &Node::onDataUpdated );

    connect( _nodeDataModel.get(),
             &NodeDataModel::embeddedWidgetSizeUpdated,
             this,
             &Node::onNodeSizeUpdated );
}

Node::~Node() {
    // Do not delete nodeDataModel whos content is non deletable
    if ( !_nodeDataModel->isDeletable() ) { _nodeDataModel.release(); }
}

QJsonObject Node::save() const {
    QJsonObject nodeJson;
#ifdef ORIGINAL_NODEEDITOR
    nodeJson["id"] = _uid.toString();

    nodeJson["model"] = _nodeDataModel->save();
#else
    nodeJson = _nodeDataModel->save();
#endif
    QJsonObject obj;
    obj["x"]             = _nodeGraphicsObject->pos().x();
    obj["y"]             = _nodeGraphicsObject->pos().y();
    nodeJson["position"] = obj;

    return nodeJson;
}

void Node::import() {
    QJsonObject json         = _nodeDataModel->save();
    _uid                     = QUuid( _nodeDataModel->uuid() );
    QJsonObject positionJson = json["position"].toObject();
    QPointF point( positionJson["x"].toDouble(), positionJson["y"].toDouble() );
    _nodeGraphicsObject->setPos( point );
}

void Node::restore( QJsonObject const& json ) {
#ifdef ORIGINAL_NODEEDITOR
    _uid                     = QUuid( json["id"].toString() );
    QJsonObject positionJson = json["position"].toObject();
    QPointF point( positionJson["x"].toDouble(), positionJson["y"].toDouble() );
    _nodeGraphicsObject->setPos( point );
    _nodeDataModel->restore( json["model"].toObject() );
#else
    _nodeDataModel->restore( json );
    _uid = QUuid( _nodeDataModel->uuid() );

    QJsonObject positionJson = json["position"].toObject();
    QPointF point( positionJson["x"].toDouble(), positionJson["y"].toDouble() );
    _nodeGraphicsObject->setPos( point );
#endif
}

QUuid Node::id() const {
    return _uid;
}

void Node::reactToPossibleConnection( PortType reactingPortType,
                                      NodeDataType const& reactingDataType,
                                      QPointF const& scenePoint ) {
    QTransform const t = _nodeGraphicsObject->sceneTransform();

    QPointF p = t.inverted().map( scenePoint );

    _nodeGeometry.setDraggingPosition( p );

    _nodeGraphicsObject->update();

    _nodeState.setReaction( NodeState::REACTING, reactingPortType, reactingDataType );
}

void Node::resetReactionToConnection() {
    _nodeState.setReaction( NodeState::NOT_REACTING );
    _nodeGraphicsObject->update();
}

NodeGraphicsObject const& Node::nodeGraphicsObject() const {
    return *_nodeGraphicsObject.get();
}

NodeGraphicsObject& Node::nodeGraphicsObject() {
    return *_nodeGraphicsObject.get();
}

void Node::setGraphicsObject( std::unique_ptr<NodeGraphicsObject>&& graphics ) {
    _nodeGraphicsObject = std::move( graphics );

    _nodeGeometry.recalculateSize();
}

NodeGeometry& Node::nodeGeometry() {
    return _nodeGeometry;
}

NodeGeometry const& Node::nodeGeometry() const {
    return _nodeGeometry;
}

NodeState const& Node::nodeState() const {
    return _nodeState;
}

NodeState& Node::nodeState() {
    return _nodeState;
}

NodeDataModel* Node::nodeDataModel() const {
    return _nodeDataModel.get();
}

void Node::propagateData( std::shared_ptr<NodeData> nodeData, PortIndex inPortIndex ) const {
    _nodeDataModel->setInData( std::move( nodeData ), inPortIndex );

    // Recalculate the nodes visuals. A data change can result in the node taking more space than
    // before, so this forces a recalculate+repaint on the affected node
    _nodeGraphicsObject->setGeometryChanged();
    _nodeGeometry.recalculateSize();
    _nodeGraphicsObject->update();
    _nodeGraphicsObject->moveConnections();
}

void Node::onDataUpdated( PortIndex index ) {
    auto nodeData = _nodeDataModel->outData( index );

    auto connections = _nodeState.connections( PortType::Out, index );

    for ( auto const& c : connections )
        c.second->propagateData( nodeData );
}

void Node::onNodeSizeUpdated() {
    if ( nodeDataModel()->embeddedWidget() ) { nodeDataModel()->embeddedWidget()->adjustSize(); }
    nodeGeometry().recalculateSize();
    for ( PortType type : { PortType::In, PortType::Out } ) {
        for ( auto& conn_set : nodeState().getEntries( type ) ) {
            for ( auto& pair : conn_set ) {
                Connection* conn = pair.second;
                conn->getConnectionGraphicsObject().move();
            }
        }
    }
}
