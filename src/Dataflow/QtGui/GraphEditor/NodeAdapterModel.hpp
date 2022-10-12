#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/QtGui/GraphEditor/ConnectionStatusData.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <nodes/NodeDataModel>

#include <iostream>

namespace Ra {
namespace Dataflow {
namespace QtGui {
namespace GraphEditor {

using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API NodeAdapterModel : public QtNodes::NodeDataModel
{
  public:
    NodeAdapterModel( DataflowGraph* graph, Node* n );
    NodeAdapterModel()                          = delete;
    NodeAdapterModel( const NodeAdapterModel& ) = delete;
    NodeAdapterModel( NodeAdapterModel&& )      = delete;
    NodeAdapterModel& operator=( const NodeAdapterModel& ) = delete;
    NodeAdapterModel& operator=( NodeAdapterModel&& ) = delete;
    ~NodeAdapterModel() override;

  public:
    QString caption() const override { return m_node->getTypeName().c_str(); }

    bool captionVisible() const override { return true; }

    QString name() const override { return m_node->getTypeName().c_str(); }

    QString uuid() const override { return m_node->getUuid().c_str(); }

    bool isDeletable() override { return true; } // Assume all nodes belong to the graph

    void updateState() override;

    void addMetaData( QJsonObject& json ) override;

  private:
    QtNodes::NodeDataType IOToDataType( size_t hashType, const std::string& ioName ) const;

    void checkConnections() const;

  public:
    unsigned int nPorts( QtNodes::PortType portType ) const override;

    QtNodes::NodeDataType dataType( QtNodes::PortType portType,
                                    QtNodes::PortIndex portIndex ) const override;

    std::shared_ptr<QtNodes::NodeData> outData( QtNodes::PortIndex port ) override;

    void setInData( std::shared_ptr<QtNodes::NodeData> data, int port ) override;

    QtNodes::NodeValidationState validationState() const override { return m_validationState; }

    QString validationMessage() const override { return m_validationError; }

    QWidget* embeddedWidget() override { return m_widget; }

  private:
    Node* m_node;
    DataflowGraph* m_dataflowGraph { nullptr };

    QWidget* m_widget { nullptr };

    std::vector<bool> m_inputsConnected;
    mutable QtNodes::NodeValidationState m_validationState = QtNodes::NodeValidationState::Valid;
    mutable QString m_validationError                      = QString( "" );

  public:
    QJsonObject save() const override;
    void restore( QJsonObject const& p ) override;
};

} // namespace GraphEditor
} // namespace QtGui
} // namespace Dataflow
} // namespace Ra
