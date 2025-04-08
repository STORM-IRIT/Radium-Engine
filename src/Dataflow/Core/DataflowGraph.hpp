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
 * \todo make a "graph embedding node" that allow to seemlesly integrate a graph as a node in
 * another graph
 *      --> Edition of a graph will allow loading and saving to a file directly
 *      --> Edition of an embeded graph will defer loading and saving to the parent graph
 *          --> for this, need to decide if a subgraph is stored in the json of its parent or in
 * a separate file
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

    /// \brief Loads nodes and links from a JSON file.
    /// \param jsonFilePath The path to the JSON file.
    /// \return true if the file was loaded, false if an error occurs.
    bool loadFromJson( const std::string& jsonFilePath );

    /// \brief Saves nodes and links to a JSON file.
    /// \param jsonFilePath The path to the JSON file.
    void saveToJson( const std::string& jsonFilePath );

    /// \brief Adds a node to the render graph.
    /// \param newNode The node to add to the graph.
    /// \return a pair with a bool and a raw pointer to the Node. If the bool is true, the raw
    /// pointer is owned by the graph. If the bool is false, the raw pointer ownership is left
    /// to the caller.
    virtual bool addNode( std::shared_ptr<Node> newNode );
    template <typename T, typename... U>
    std::shared_ptr<T> addNode( U&&... u );

    /// \brief Removes a node from the render graph.
    /// \param node The node to remove from the graph.
    /// \return true if the node was removed and the given pointer is set to nullptr, false else
    virtual bool removeNode( std::shared_ptr<Node> node );

    /// Connects two nodes of the graph.
    /// The two nodes must already be in the graph (with the addNode(Node* newNode)
    /// function), in order to be linked the first node's in port must be free and the connected
    /// in port and out port must have the same type of data.
    ///
    /// \param nodeFrom The node that contains the out port.
    /// \param nodeFromOutputName The name of the out port in nodeFrom.
    /// \param nodeTo The node that contains the in port.
    /// \param nodeToInputName The name of the in port in nodeTo.
    /// \return true if link added, false if link could not be made.
    bool addLink( const std::shared_ptr<Node>& nodeFrom,
                  const std::string& nodeFromOutputName,
                  const std::shared_ptr<Node>& nodeTo,
                  const std::string& nodeToInputName );

    bool addLink( const std::shared_ptr<Node>& nodeFrom,
                  Node::PortIndex portOutIdx,
                  const std::shared_ptr<Node>& nodeTo,
                  Node::PortIndex portInIdx );

    bool addLink( Node::PortBaseOutRawPtr outputPort, Node::PortBaseInRawPtr inputPort );

    template <typename T, typename U>
    bool addLink( const std::shared_ptr<PortOut<T>>& outputPort,
                  const std::shared_ptr<PortIn<U>>& inputPort );

    bool canLink( const std::shared_ptr<Node>& nodeFrom,
                  Node::PortIndex portOutIdx,
                  const std::shared_ptr<Node>& nodeTo,
                  Node::PortIndex portInIdx ) const {
        return canLink( nodeFrom.get(), portOutIdx, nodeTo.get(), portInIdx );
    }

    bool canLink( const Node* nodeFrom,
                  Node::PortIndex portOutIdx,
                  const Node* nodeTo,
                  Node::PortIndex portInIdx ) const {
        auto portIn  = nodeTo->getInputByIndex( portInIdx );
        auto portOut = nodeFrom->getOutputByIndex( portOutIdx );

        if ( !checkNodeValidity( nodeFrom, nodeTo ) ) { return false; }
        if ( check_last_port_io_nodes( nodeFrom, portOutIdx, nodeTo, portInIdx ) ) {
            if ( nodeFrom == m_input_node.get() ) return portIn != nullptr;
            if ( nodeTo == m_output_node.get() ) return portOut != nullptr;
        }

        // Compare types
        return portIn && portOut &&
               ( portIn->getType() == portOut->getType() && !portIn->isLinked() );
    }

    /** \brief Removes the link connected to a node's input port
     *
     * \param node the node to unlink
     * \param nodeInputName the name of the port to unlink
     * \return true if link is removed, false if not.
     */

    bool removeLink( std::shared_ptr<Node> node, const std::string& nodeInputName );

    /** \copybrief removeLink
     *
     * \param node the node to unlink
     * \param in_port_index index of the port's input port to unlink
     * \return true if link is removed, false if not.
     */
    bool removeLink( std::shared_ptr<Node> node, const PortIndex& in_port_index );

    /// \brief Get the vector of all the nodes on the graph
    /// \return
    const std::vector<std::shared_ptr<Node>>& getNodes() const { return m_nodes; }

    /// Gets a specific node according to its instance name.
    /// \param instanceNameNode The instance name of the node.
    std::shared_ptr<Node> getNode( const std::string& instanceNameNode ) const;
    /** \copydoc getNode()
     *
     * Dynamic cast of the node pointer to T
     *
     * \tparam T Node's type to cast node to.
     */
    template <typename T>
    std::shared_ptr<T> getNode( const std::string& instanceNameNode ) const {
        return std::dynamic_pointer_cast<T>( getNode( instanceNameNode ) );
    }

    /// Gets the nodes ordered by level (after compilation)
    const std::vector<std::vector<Node*>>& getNodesByLevel() const { return m_nodesByLevel; }

    /// Compile the render graph to check its validity and simplify it.
    /// The compilation has multiple goals:
    /// - Remove the nodes that have no direct or indirect connections to sink nodes
    /// - Order the nodes by level according to their dependencies
    /// - Check if every mandatory port is linked
    bool compile() override;

    /** fill input and output ports of graph frome its nodes.
     */
    void generate_ports();

    /// Gets the number of nodes
    size_t getNodesCount() const { return m_nodes.size(); }

    /// Deletes all nodes from the render graph.
    virtual void clearNodes();

    /// Test if the graph is compiled
    bool isCompiled() const { return m_ready; }

    /// Mark the graph as needing recompilation (useful to force recompilation and resources
    /// update)
    inline void needsRecompile();

    /// \brief Gets an input port form a node of the graph.
    /// Return the port if exists.
    /// This input port could then be used through setter->setDefaultValue( data ) to set the graph
    /// input from the data.
    /// \note The raw pointer is only valid as graph is valid.
    /// \param nodeNome The name of the node
    /// \param portName The name of the input port
    Node::PortBaseInRawPtr getNodeInputPort( const std::string& nodeName,
                                             const std::string& portName ) {
        auto node = getNode( nodeName );
        auto port = node->getInputByName( portName );
        CORE_ASSERT( port.first.isValid(),
                     "invalid port, node: " + nodeName + " port: " + portName );
        return port.second;
    }

    /// \brief Gets an output port from a node of the graph.
    /// Allows to get the data stored at this port after the execution of the graph.
    /// The return port can be use as in port->getData().
    /// \note ownership is left to the graph, not shared. The graph must survive the returned
    /// raw pointer to be able to use the dataGetter.
    /// \param nodeNome The name of the node
    /// \param portName The name of the output port
    Node::PortBaseOutRawPtr getNodeOutputPort( const std::string& nodeName,
                                               const std::string& portName ) {
        auto node = getNode( nodeName );
        auto port = node->getOutputByName( portName );
        CORE_ASSERT( port.first.isValid(),
                     "invalid port, node: " + nodeName + " port: " + portName );

        return port.second;
    }

    bool shouldBeSaved() { return m_shouldBeSaved; }

    static const std::string& getTypename();

    /**
     * \brief Load a graph from the given file.
     * \param filename
     * Any type of graph that inherits from DataflowGraph can be loaded by this function as soon
     * as the appropriate constructor is registered in Ra::Dataflow::NodeFactoriesManager.
     * \return The loaded graph, as a DataFlowGraph pointer to be downcast to the correct type
     */
    static std::shared_ptr<DataflowGraph> loadGraphFromJsonFile( const std::string& filename );

    /**
     * \brief protect nodes and links from deletion.
     * \param on true to protect, false to unprotect.
     */
    void setNodesAndLinksProtection( bool on ) { m_nodesAndLinksProtected = on; }

    /**
     * \brief get the protection status protect nodes and links from deletion
     * \return the protection status
     */
    bool getNodesAndLinksProtection() const { return m_nodesAndLinksProtected; }

    using Node::addInput;
    using Node::addInputPort;
    using Node::addOutput;
    using Node::addOutputPort;

    void add_input_output_nodes() {
        if ( !m_input_node ) { m_input_node = std::make_shared<GraphInputNode>( "input" ); }
        if ( !m_output_node ) { m_output_node = std::make_shared<GraphOutputNode>( "output" ); }
        m_input_node->set_graph( this );
        m_output_node->set_graph( this );
        addNode( m_input_node );
        addNode( m_output_node );
    }

    // invalidate port indices
    void remove_unlinked_input_output_ports() {
        if ( m_input_node ) { m_input_node->remove_unlinked_ports(); }
        if ( m_output_node ) { m_output_node->remove_unlinked_ports(); }
        generate_ports();
    }

    std::shared_ptr<GraphOutputNode> output_node() { return m_output_node; }
    std::shared_ptr<GraphInputNode> input_node() { return m_input_node; }

  protected:
    /** Allow derived class to construct the graph with their own static type
     */
    DataflowGraph( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& data ) override;
    void toJsonInternal( nlohmann::json& ) const override;

    /** Check if there node with same instance and model is in the graph.
     *
     * \param instance Instance name to search
     * \param model Model name to search
     */
    bool findNode( const std::string& instance, const std::string& model ) const;
    /** Check if node is part of the graph, or part of its "inner" graph.
     * \param node Raw pointer of the node to find.
     */
    bool findNodeDeep( const Node* node ) const;

  private:
    // Internal helper functions
    /// Internal compilation function that allows to go back in the render graph while filling
    /// an information map. \param current The current node. \param infoNodes The map that
    /// contains information about nodes.
    void backtrackGraph( Node* current,
                         std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes );
    /// Internal compilation function that allows to go through the graph, using an
    /// information map.
    /// \param current The current node.
    /// \param infoNodes The map that contains information about nodes.
    int goThroughGraph( Node* current,
                        std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes );

    bool check_last_port_io_nodes( const Node* nodeFrom,
                                   Node::PortIndex portOutIdx,
                                   const Node* nodeTo,
                                   Node::PortIndex portInIdx ) const {
        if ( nodeFrom == m_input_node.get() && portOutIdx == m_input_node->getOutputs().size() )
            return true;
        if ( nodeTo == m_output_node.get() && portInIdx == m_output_node->getInputs().size() )
            return true;
        return false;
    }

    bool checkNodeValidity( const Node* nodeFrom, const Node* nodeTo, bool verbose = false ) const;
    static bool checkPortCompatibility( const Node* nodeFrom,
                                        const PortBaseOut* portOut,
                                        const Node* nodeTo,
                                        const PortBaseIn* portIn );
    class RA_DATAFLOW_CORE_API Log
    {
      public:
        static void alreadyLinked( const Node* node, const PortBase* port );
        static void addLinkTypeMismatch( const Node* nodeFrom,
                                         const PortBase* portOut,
                                         const Node* nodeTo,
                                         const PortBase* portIn );
        static void unableToFind( const std::string& type, const std::string& instanceName );
        static void
        badPortIdx( const std::string& type, const std::string& instanceName, Node::PortIndex idx );
        static void try_to_link_input_to_output();
    };

    /// Flag that indicates if the graph should be saved to a file
    /// This flag is useless outside an load/edit/save scenario
    bool m_shouldBeSaved { false };

    /// Flag set after successful compilation indicating graph is ready to be executed
    /// This flag is reset as soon as the graph is modified.
    bool m_ready { false };

    /// The unordered list of nodes.
    std::vector<std::shared_ptr<Node>> m_nodes;
    std::shared_ptr<GraphOutputNode> m_output_node { nullptr };
    std::shared_ptr<GraphInputNode> m_input_node { nullptr };
    // Internal node levels representation
    /// The list of nodes ordered by levels.
    /// Two nodes at the same level have no dependency between them.
    std::vector<std::vector<Node*>> m_nodesByLevel;

    bool m_nodesAndLinksProtected { false };
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename T, typename... U>
std::shared_ptr<T> DataflowGraph::addNode( U&&... u ) {
    auto ret = std::make_shared<T>( std::forward<U>( u )... );
    if ( addNode( ret ) ) return ret;
    return nullptr;
}

template <typename T, typename U>
bool DataflowGraph::addLink( const std::shared_ptr<PortOut<T>>& outputPort,
                             const std::shared_ptr<PortIn<U>>& inputPort ) {
    using namespace Ra::Core::Utils;

    static_assert( std::is_same_v<T, U>, "in and out port's types mismatch" );

    return addLink( outputPort.get(), inputPort.get() );
}

inline void DataflowGraph::needsRecompile() {
    m_shouldBeSaved = true;
    m_ready         = false;
}

inline const std::string& DataflowGraph::getTypename() {
    static std::string demangledTypeName { "Core DataflowGraph" };
    return demangledTypeName;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
