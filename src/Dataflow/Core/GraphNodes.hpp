#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <Core/Types.hpp>
#include <Dataflow/Core/Node.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

#define BASIC_NODE_INIT( TYPE, BASE )                                                 \
    explicit TYPE( const std::string& name ) : TYPE( name, TYPE::node_typename() ) {} \
    RA_NODE_TYPENAME( #TYPE )                                                         \
    TYPE( const std::string& instanceName, const std::string& typeName ) :            \
        BASE( instanceName, typeName ) {}

class RA_DATAFLOW_CORE_API GraphNode : public Node
{
  public:
    BASIC_NODE_INIT( GraphNode, Node );

    bool execute() override;
    void remove_unlinked_ports();
    void set_graph( Node* node ) { m_graph = node; }
    Node* graph() const { return m_graph; }

  protected:
    /**
     * Add input and output port suitable to connect the same type as port in argument, with a name
     * related to port->name(), but uniquely define in this node, e.g. by adding a numerical suffix
     * if needed, "_1" for instance.
     */
    auto add_ports( PortBaseRawPtr port )
        -> std::tuple<PortIndex, PortIndex, PortBaseInPtr, PortBaseOutPtr>;
    auto find_available_name( const std::string& type, const std::string& name ) -> std::string;
    bool fromJsonInternal( const nlohmann::json& data ) override;

  private:
    Node* m_graph { nullptr };
};

class RA_DATAFLOW_CORE_API GraphInputNode : public GraphNode
{
  public:
    BASIC_NODE_INIT( GraphInputNode, GraphNode );

    /**
     * Given an input port (from another node) add an output port to this GraphInputNode and
     * connect the newly created output port to input port in argument
     **/
    auto add_output_port( PortBaseInRawPtr port ) -> PortIndex;
};

class RA_DATAFLOW_CORE_API GraphOutputNode : public GraphNode
{
  public:
    BASIC_NODE_INIT( GraphOutputNode, GraphNode );

    /**
     * Given an output port (from another node) add an input port to this GraphOutputNode and
     * connect the newly created input port to output port in argument
     **/
    auto add_input_port( PortBaseOutRawPtr port ) -> PortIndex;
};
} // namespace Core
} // namespace Dataflow
} // namespace Ra
