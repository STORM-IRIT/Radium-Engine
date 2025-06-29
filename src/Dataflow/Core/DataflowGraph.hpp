#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/GraphNodes.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/BijectiveAssociation.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Singleton.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {

/**
 * \brief Represent a set of connected nodes that define a Direct Acyclic Computational Graph
 * Ownership of nodes is given to the graph at construction time.
 */
class RA_DATAFLOW_CORE_API DataflowGraph : public Node
{
  public:
    /** The nodes pointing to external data are created here.
     * \param name The name of the render graph.
     */
    explicit DataflowGraph( const std::string& name );
    virtual ~DataflowGraph() = default;

    void init() override;
    bool execute() override;
    void destroy() override;

    /**
     * \brief Loads nodes and links from a JSON file.
     *
     * \param jsonFilePath The path to the JSON file.
     * \return true if the file was loaded, false if an error occurs.
     */
    bool loadFromJson( const std::string& jsonFilePath );

    /**
     * \brief Saves nodes and links to a JSON file.
     *
     * \param jsonFilePath The path to the JSON file.
     */
    void saveToJson( const std::string& jsonFilePath );

    /**
     * \brief Adds a node to the graph.
     *
     * \param newNode The node to add to the graph.
     * \return true if node has been added to the graph (no other node with same model and instance
     * name), false otherwise.
     */
    virtual bool add_node( std::shared_ptr<Node> newNode );
    /// Conveniance typed alias
    template <typename T, typename... U>
    std::shared_ptr<T> add_node( U&&... u );

    /**
     * \brief Removes a node from the graph.
     * \param node The node to remove from the graph.
     * \return true if the node was removed.
     */
    virtual bool remove_node( std::shared_ptr<Node> node );

    /**
     * \brief Connects two nodes of the graph.
     *
     * The two nodes must already be in the graph (with the add_node), in order to be linked the
     * seconde node's in port must be free and the connected in port and out port must have the same
     * type of data.
     *
     * \param nodeFrom The node that contains the out port.
     * \param nodeFromOutputName The name of the out port in nodeFrom.
     * \param nodeTo The node that contains the in port.
     * \param nodeToInputName The name of the in port in nodeTo.
     * \return true if link added, false if link could not be made.
     */
    bool add_link( const std::shared_ptr<Node>& nodeFrom,
                   const std::string& nodeFromOutputName,
                   const std::shared_ptr<Node>& nodeTo,
                   const std::string& nodeToInputName );
    /// Convenience alias using port index
    bool add_link( const std::shared_ptr<Node>& nodeFrom,
                   Node::PortIndex portOutIdx,
                   const std::shared_ptr<Node>& nodeTo,
                   Node::PortIndex portInIdx );
    /// Convenience alias using port raw ptr, checks if ports' nodes are in the graph.
    bool add_link( Node::PortBaseOutRawPtr outputPort, Node::PortBaseInRawPtr inputPort );

    /// Convenience typed alias
    template <typename T, typename U>
    bool add_link( const std::shared_ptr<PortOut<T>>& outputPort,
                   const std::shared_ptr<PortIn<U>>& inputPort );

    /// \return true if ports can be linked
    bool can_link( const std::shared_ptr<Node>& nodeFrom,
                   Node::PortIndex portOutIdx,
                   const std::shared_ptr<Node>& nodeTo,
                   Node::PortIndex portInIdx ) const;

    /// Convenience alias using raw pointer.
    bool can_link( const Node* nodeFrom,
                   Node::PortIndex portOutIdx,
                   const Node* nodeTo,
                   Node::PortIndex portInIdx ) const;

    /**
     * \brief Removes the link connected to a node's input port
     *
     * \param node the node to unlink
     * \param nodeInputName the name of the port to unlink
     * \return true if link is removed, false if not.
     */
    bool remove_link( std::shared_ptr<Node> node, const std::string& nodeInputName );

    /** \copybrief remove_link
     *
     * \param node the node to unlink
     * \param in_port_index index of the port's input port to unlink
     * \return true if link is removed, false if not.
     */
    bool remove_link( std::shared_ptr<Node> node, const PortIndex& in_port_index );

    /// \brief Get the vector of all the nodes on the graph
    const std::vector<std::shared_ptr<Node>>& nodes() const { return m_nodes; }

    /// Gets a specific node according to its instance name.
    /// \param instanceNameNode The instance name of the node.
    std::shared_ptr<Node> node( const std::string& instanceNameNode ) const;

    /** \copydoc node()
     *
     * Dynamic cast of the node pointer to T
     *
     * \tparam T Node's type to cast node to.
     */
    template <typename T>
    std::shared_ptr<T> node( const std::string& instanceNameNode ) const {
        return std::dynamic_pointer_cast<T>( node( instanceNameNode ) );
    }

    /// Gets the nodes ordered by level (after compilation)
    const std::vector<std::vector<Node*>>& nodes_by_level() const { return m_nodes_by_level; }

    /**
     * \brief Compile the graph to check its validity and simplify it.
     *
     * The compilation has multiple goals:
     * - Remove the nodes that have no direct or indirect connections to sink nodes
     * - Order the nodes by level according to their dependencies
     * - Check if every mandatory port is linked
     */
    bool compile() override;

    /**
     * \brief fill input and output ports of graph from its input and output nodes if exists.
     *
     * \see add_input_output_nodes()
     */
    void generate_ports();

    /// Gets the number of nodes
    size_t node_count() const { return m_nodes.size(); }

    /// Deletes all nodes from the render graph.
    virtual void clear_nodes();

    /// Test if the graph is compiled
    bool is_compiled() const { return m_ready; }

    /// Mark the graph as needing recompilation (useful to force recompilation and resources
    /// update)
    inline void needs_recompile();

    /**
     * \brief Gets an input port form a node of the graph.
     *
     * This input port could then be used through setter->set_default_value( data ) to set the
     * graph input from the data.
     * \note The raw pointer is only valid as graph is valid.
     * \param nodeNome The name of the node
     * \param portName The name of the input port
     * \return the port if exists, nullptr otherwise.
     */
    Node::PortBaseInRawPtr input_node_port( const std::string& nodeName,
                                            const std::string& portName );
    /**
     * \brief Gets an output port from a node of the graph.
     *
     * Allows to get the data stored at this port after the execution of the graph.
     * The return port can be use as in port->data().
     * \note ownership is left to the graph, not shared. The graph must survive the returned
     * raw pointer to be able to use the dataGetter.
     * \param nodeNome The name of the node
     * \param portName The name of the output port
     * \return the port if exists, nullptr otherwise.
     */
    Node::PortBaseOutRawPtr output_node_port( const std::string& nodeName,
                                              const std::string& portName );

    bool shouldBeSaved() { return m_should_save; }

    static const std::string& node_typename();

    /**
     * \brief Load a graph from the given file.
     *
     * Any type of graph that inherits from DataflowGraph can be loaded by this function as soon
     * as the appropriate constructor is registered in Ra::Dataflow::NodeFactoriesManager.
     * \param filename
     * \return The loaded graph, as a DataFlowGraph pointer to be downcast to the correct type
     */
    static std::shared_ptr<DataflowGraph> loadGraphFromJsonFile( const std::string& filename );

    /**
     * \brief protect nodes and links from deletion.
     * \param on true to protect, false to unprotect.
     * \todo should this be only on gui side ?
     */
    void setNodesAndLinksProtection( bool on ) { m_nodesAndLinksProtected = on; }

    /**
     * \brief get the protection status protect nodes and links from deletion
     * \return the protection status
     */
    bool nodesAndLinksProtection() const { return m_nodesAndLinksProtected; }

    using Node::add_input;
    using Node::add_output;

    /**
     * \brief Create (if not already created) input/output node of the graph, and fills graph
     * input/output.
     *
     * These nodes are usefull for using graph as node and stating the graph as node input/output
     * ports.
     * ![example inner graph](images/graph_as_node_inner.png)
     */
    void add_input_output_nodes();

    /**
     * \brief Removes unsused (unlinked) input/output ports.
     *
     * A (e.g. input) port is unlink, if the corresponding ports of input_node is unlink both on
     * input (outside the graph) and input (inside the graph).
     * \note invalidate port indices
     */
    void remove_unlinked_input_output_ports();

    std::shared_ptr<GraphOutputNode> output_node() { return m_output_node; }
    std::shared_ptr<GraphInputNode> input_node() { return m_input_node; }

  protected:
    /**
     * \brief Allow derived class to construct the graph with their own static type.
     */
    DataflowGraph( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& data ) override;
    void toJsonInternal( nlohmann::json& ) const override;

    /**
     * \brief Check if there node with same instance and model is in the graph.
     *
     * \param instance Instance name to search
     * \param model Model name to search
     */
    bool has_node_by_name( const std::string& instance, const std::string& model ) const;
    /**
     * \brief Check if node is part of the graph, or part of its "inner" graph.
     *
     * \param node Raw pointer of the node to find.
     */
    bool contains_node_recursive( const Node* node ) const;

  private:
    // Internal helper functions
    /// Internal compilation function that allows to go back in the render graph while filling
    /// an information map. \param current The current node. \param infoNodes The map that
    /// contains information about nodes.
    void
    backtrack_graph( Node* current,
                     std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes );
    /// Internal compilation function that allows to go through the graph, using an
    /// information map.
    /// \param current The current node.
    /// \param infoNodes The map that contains information about nodes.
    int traverse_graph( Node* current,
                        std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes );

    /// to allow dynamic creation of ports on input/output nodes, only possible new port is last
    /// port of the given node.
    bool check_last_port_io_nodes( const Node* nodeFrom,
                                   Node::PortIndex portOutIdx,
                                   const Node* nodeTo,
                                   Node::PortIndex portInIdx ) const {
        if ( nodeFrom == m_input_node.get() && portOutIdx == m_input_node->outputs().size() )
            return true;
        if ( nodeTo == m_output_node.get() && portInIdx == m_output_node->inputs().size() )
            return true;
        return false;
    }

    bool are_nodes_valids( const Node* nodeFrom, const Node* nodeTo, bool verbose = false ) const;
    static bool are_ports_compatible( const Node* nodeFrom,
                                      const PortBaseOut* portOut,
                                      const Node* nodeTo,
                                      const PortBaseIn* portIn );
    class RA_DATAFLOW_CORE_API Log
    {
      public:
        static void already_linked( const Node* node, const PortBase* port );
        static void link_type_mismatch( const Node* nodeFrom,
                                        const PortBase* portOut,
                                        const Node* nodeTo,
                                        const PortBase* portIn );
        static void unable_to_find( const std::string& type, const std::string& instanceName );
        static void bad_port_index( const std::string& type,
                                    const std::string& instanceName,
                                    Node::PortIndex idx );
        static void try_to_link_input_to_output();
    };

    /// Flag that indicates if the graph should be saved to a file
    /// This flag is useless outside an load/edit/save scenario
    bool m_should_save { false };

    /// Flag set after successful compilation indicating graph is ready to be executed
    /// This flag is reset as soon as the graph is modified.
    bool m_ready { false };

    /// The unordered list of nodes.
    std::vector<std::shared_ptr<Node>> m_nodes;
    std::shared_ptr<GraphOutputNode> m_output_node { nullptr };
    std::shared_ptr<GraphInputNode> m_input_node { nullptr };

    /// The list of nodes ordered by levels.
    /// Two nodes at the same level have no dependency between them.
    std::vector<std::vector<Node*>> m_nodes_by_level;

    bool m_nodesAndLinksProtected { false };
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename T, typename... U>
std::shared_ptr<T> DataflowGraph::add_node( U&&... u ) {
    auto ret = std::make_shared<T>( std::forward<U>( u )... );
    if ( add_node( ret ) ) return ret;
    return nullptr;
}

template <typename T, typename U>
bool DataflowGraph::add_link( const std::shared_ptr<PortOut<T>>& outputPort,
                              const std::shared_ptr<PortIn<U>>& inputPort ) {
    using namespace Ra::Core::Utils;

    static_assert( std::is_same_v<T, U>, "in and out port's types mismatch" );

    return add_link( outputPort.get(), inputPort.get() );
}

inline bool DataflowGraph::can_link( const std::shared_ptr<Node>& nodeFrom,
                                     Node::PortIndex portOutIdx,
                                     const std::shared_ptr<Node>& nodeTo,
                                     Node::PortIndex portInIdx ) const {
    return can_link( nodeFrom.get(), portOutIdx, nodeTo.get(), portInIdx );
}

inline bool DataflowGraph::can_link( const Node* nodeFrom,
                                     Node::PortIndex portOutIdx,
                                     const Node* nodeTo,
                                     Node::PortIndex portInIdx ) const {
    auto portIn  = nodeTo->input_by_index( portInIdx );
    auto portOut = nodeFrom->output_by_index( portOutIdx );

    if ( !are_nodes_valids( nodeFrom, nodeTo ) ) { return false; }
    if ( check_last_port_io_nodes( nodeFrom, portOutIdx, nodeTo, portInIdx ) ) {
        if ( nodeFrom == m_input_node.get() ) return portIn != nullptr;
        if ( nodeTo == m_output_node.get() ) return portOut != nullptr;
    }

    // Compare types
    return portIn && portOut && ( portIn->type() == portOut->type() && !portIn->is_linked() );
}

inline void DataflowGraph::needs_recompile() {
    m_should_save = true;
    m_ready       = false;
}

inline Node::PortBaseInRawPtr DataflowGraph::input_node_port( const std::string& nodeName,
                                                              const std::string& portName ) {
    auto n = node( nodeName );
    auto p = n->input_by_name( portName );
    CORE_ASSERT( p.first.isValid(), "invalid port, node: " + nodeName + " port: " + portName );
    return p.second;
}

inline Node::PortBaseOutRawPtr DataflowGraph::output_node_port( const std::string& nodeName,
                                                                const std::string& portName ) {
    auto n = node( nodeName );
    auto p = n->output_by_name( portName );
    CORE_ASSERT( p.first.isValid(), "invalid port, node: " + nodeName + " port: " + portName );

    return p.second;
}

inline void DataflowGraph::add_input_output_nodes() {
    if ( !m_input_node ) { m_input_node = std::make_shared<GraphInputNode>( "input" ); }
    if ( !m_output_node ) { m_output_node = std::make_shared<GraphOutputNode>( "output" ); }
    m_input_node->set_graph( this );
    m_output_node->set_graph( this );
    add_node( m_input_node );
    add_node( m_output_node );
}

inline void DataflowGraph::remove_unlinked_input_output_ports() {
    if ( m_input_node ) { m_input_node->remove_unlinked_ports(); }
    if ( m_output_node ) { m_output_node->remove_unlinked_ports(); }
    generate_ports();
}

inline const std::string& DataflowGraph::node_typename() {
    static std::string demangledTypeName { "Core DataflowGraph" };
    return demangledTypeName;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
