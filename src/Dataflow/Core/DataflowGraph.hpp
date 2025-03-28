#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Enumerator.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>

#include <Core/Types.hpp>
#include <Core/Utils/BijectiveAssociation.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Singleton.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortFactory
{
    RA_SINGLETON_INTERFACE( PortFactory );

  public:
    using PortInCtorFunctor  = std::function<Node::PortBaseInPtr( Node*, const std::string& )>;
    using PortOutCtorFunctor = std::function<Node::PortBaseOutPtr( Node*, const std::string& )>;
    using PortOutSetter      = std::function<void( PortBaseOut*, std::any )>;
    using PortInGetter       = std::function<std::any( PortBaseIn* )>;

    Node::PortBaseInPtr
    make_input_port( Node* node, const std::string& name, std::type_index type ) {
        return m_input_ctor.at( type )( node, name );
    }
    Node::PortBaseOutPtr
    make_output_port( Node* node, const std::string& name, std::type_index type ) {
        return m_output_ctor.at( type )( node, name );
    }

    Node::PortBaseInPtr make_input_port( Node* node, const std::string& name, std::string type ) {
        return make_input_port( node, name, m_type_to_string.key( type ) );
    }
    Node::PortBaseOutPtr make_output_port( Node* node, const std::string& name, std::string type ) {
        return make_output_port( node, name, m_type_to_string.key( type ) );
    }

    PortOutSetter output_setter( std::type_index type ) { return m_output_setter[type]; }
    PortInGetter input_getter( std::type_index type ) { return m_input_getter[type]; }

    template <typename T>
    void add_port_type() {
        auto type          = std::type_index( typeid( T ) );
        m_input_ctor[type] = []( Node* node, const std::string& name ) {
            return std::make_shared<PortIn<T>>( node, name );
        };

        m_output_ctor[type] = []( Node* node, const std::string& name ) {
            return std::make_shared<PortOut<T>>( node, name );
        };

        m_input_getter[type] = []( PortBaseIn* port ) -> std::any {
            auto casted = dynamic_cast<PortIn<T>*>( port );
            return &( casted->getData() );
        };
        m_output_setter[type] = []( PortBaseOut* port, std::any any ) {
            T* data     = std::any_cast<T*>( any );
            auto casted = dynamic_cast<PortOut<T>*>( port );
            casted->setData( data );
        };

        m_type_to_string.insert( type, Ra::Core::Utils::simplifiedDemangledType( type ) );
    }

  private:
    PortFactory() {
        using namespace Ra::Core;
        add_port_type<Scalar>();
        add_port_type<int>();
        add_port_type<unsigned int>();
        add_port_type<Utils::Color>();
        add_port_type<Vector2>();
        add_port_type<Vector3>();
        add_port_type<Vector4>();
    }

    std::unordered_map<std::type_index, PortInCtorFunctor> m_input_ctor;
    std::unordered_map<std::type_index, PortInGetter> m_input_getter;
    std::unordered_map<std::type_index, PortOutCtorFunctor> m_output_ctor;
    std::unordered_map<std::type_index, PortOutSetter> m_output_setter;

    Ra::Core::Utils::BijectiveAssociation<std::type_index, std::string> m_type_to_string;
};

#define BASIC_NODE_INIT( TYPE, BASE )                                               \
  public:                                                                           \
    explicit TYPE( const std::string& name ) : TYPE( name, TYPE::getTypename() ) {} \
    static const std::string& getTypename() {                                       \
        static std::string demangledName = #TYPE;                                   \
        return demangledName;                                                       \
    }                                                                               \
    TYPE( const std::string& instanceName, const std::string& typeName ) :          \
        BASE( instanceName, typeName )

class GraphNode : public Node
{
    BASIC_NODE_INIT( GraphNode, Node ) {}

  public:
    bool execute() override {
        /// \todo add assert check on size ?
        for ( size_t i = 0; i < m_inputs.size(); ++i ) {
            auto factory       = PortFactory::getInstance();
            auto output_setter = factory->output_setter( m_outputs[i]->getType() );
            auto input_getter  = factory->input_getter( m_inputs[i]->getType() );
            output_setter( m_outputs[i].get(), input_getter( m_inputs[i].get() ) );
        }
        return true;
    }

    void remove_unlinked_ports() {
        int last_index = m_inputs.size();
        for ( int i = 0; i < last_index; ++i ) {
            if ( !m_inputs[i]->isLinked() && m_outputs[i]->getLinkCount() == 0 ) {
                std::swap( m_inputs[i], m_inputs[last_index - 1] );
                std::swap( m_outputs[i], m_outputs[last_index - 1] );
                --last_index;
                --i;
            }
        }
        m_inputs.erase( m_inputs.begin() + last_index, m_inputs.end() );
        m_outputs.erase( m_outputs.begin() + last_index, m_outputs.end() );
    }

    void set_graph( Node* node ) { m_graph = node; }
    Node* graph() const { return m_graph; }

  protected:
    auto add_ports( PortBaseRawPtr port ) {
        auto factory    = PortFactory::getInstance();
        auto in_name    = find_available_name( "in", port->getName() );
        auto in         = factory->make_input_port( this, in_name, port->getType() );
        auto out_name   = find_available_name( "out", port->getName() );
        auto out        = factory->make_output_port( this, out_name, port->getType() );
        auto input_idx  = addInput( in );
        auto output_idx = addOutput( out );
        return std::make_tuple( input_idx, output_idx, in, out );
    }

    auto find_available_name( const std::string& type, const std::string& name ) -> std::string {
        int suffix           = 1;
        std::string new_name = name;
        while ( getPortByName( type, new_name ).first.isValid() ) {
            new_name = name + "_" + std::to_string( suffix++ );
        }
        return new_name;
    }

    bool fromJsonInternal( const nlohmann::json& data ) override {
        auto factory = PortFactory::getInstance();
        std::map<size_t, PortBaseInPtr> inputs;
        std::map<size_t, PortBaseOutPtr> outputs;
        if ( const auto& ports = data.find( "inputs" ); ports != data.end() ) {
            for ( const auto& port : *ports ) {
                size_t index     = port["port_index"];
                std::string type = port["type"];
                std::string name = port["name"];
                inputs[index]    = factory->make_input_port( this, name, type );
            }
        }
        if ( const auto& ports = data.find( "outputs" ); ports != data.end() ) {
            for ( const auto& port : *ports ) {
                size_t index     = port["port_index"];
                std::string type = port["type"];
                std::string name = port["name"];
                outputs[index]   = factory->make_output_port( this, name, type );
            }
        }

        m_inputs.clear();
        m_outputs.clear();
        for ( const auto& [key, value] : inputs ) {
            assert( m_inputs.size() == key );
            m_inputs.push_back( value );
        }
        for ( const auto& [key, value] : outputs ) {
            assert( m_outputs.size() == key );
            m_outputs.push_back( value );
        }
        assert( m_inputs.size() == m_outputs.size() );

        return true;
    }

  private:
    Node* m_graph { nullptr };
};

class GraphInputNode : public GraphNode
{
    BASIC_NODE_INIT( GraphInputNode, GraphNode ) {}

  public:
    PortIndex add_output_port( PortBaseInRawPtr port ) {
        auto [input_idx, output_idx, in, out] = add_ports( port );
        port->connect( out.get() );
        return input_idx;
    }
};

class GraphOutputNode : public GraphNode
{
    BASIC_NODE_INIT( GraphOutputNode, GraphNode ) {}

  public:
    PortIndex add_input_port( PortBaseOutRawPtr port ) {
        auto [input_idx, output_idx, in, out] = add_ports( port );

        in->connect( port );
        return output_idx;
    }
};

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
class RA_DATAFLOW_API DataflowGraph : public Node
{
  public:
    /// Constructor.
    /// The nodes pointing to external data are created here.
    /// \param name The name of the render graph.
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
        ///\todo check node and port as in addLink, to replace addLink checks
        if ( !checkNodeValidity( nodeFrom, nodeTo ) ) { return false; }

        if ( ( nodeFrom == m_input_node.get() || nodeFrom == m_output_node.get() ) &&
             ( nodeTo == m_input_node.get() || nodeTo == m_output_node.get() ) )
            return false;
        if ( nodeFrom == m_input_node.get() && portOutIdx == m_input_node->getOutputs().size() )
            return true;
        if ( nodeTo == m_output_node.get() && portInIdx == m_output_node->getInputs().size() )
            return true;

        auto portIn  = nodeTo->getInputByIndex( portInIdx );
        auto portOut = nodeFrom->getOutputByIndex( portOutIdx );

        // Compare types
        return portIn && portOut && ( portIn->getType() == portOut->getType() ) &&
               !portIn->isLinked();
    }

    ///
    /// \brief Removes the link connected to a node's input port
    /// \param node the node to unlink
    /// \param nodeInputName the name of the port to unlink
    /// \return true if link is removed, false if not.
    bool removeLink( std::shared_ptr<Node>, const std::string& nodeInputName );
    bool removeLink( std::shared_ptr<Node> node, const PortIndex& in_port_index ) {
        node->getInputs()[in_port_index]->disconnect();
        needsRecompile();
        return true;
    }

    /// \brief Get the vector of all the nodes on the graph
    /// \return
    const std::vector<std::shared_ptr<Node>>& getNodes() const { return m_nodes; }

    /// Gets a specific node according to its instance name.
    /// \param instanceNameNode The instance name of the node.
    std::shared_ptr<Node> getNode( const std::string& instanceNameNode ) const;

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

    /// \brief Gets an output port connected to the named input port of the graph.
    /// Return the connected output port if success, sharing the ownership with the caller.
    /// This output port could then be used through setter->setData( ptr ) to set the graph
    /// input from the data pointer owned by the caller. \note As ownership is shared with the
    /// caller, the graph must survive the returned pointer to be able to use the dataSetter..
    /// \params portName The name of the input port of the graph

    Node::PortBaseInRawPtr getDataSetter( const std::string& nodeName,
                                          const std::string& portName ) {
        auto node = getNode( nodeName );
        auto port = node->getInputByName( portName );
        CORE_ASSERT( port.first.isValid(),
                     "invalid port, node: " + nodeName + " port: " + portName );
        return port.second;
    }

    /// \brief Returns an alias to the named output port of the graph.
    /// Allows to get the data stored at this port after the execution of the graph.
    /// \note ownership is left to the graph, not shared. The graph must survive the returned
    /// pointer to be able to use the dataGetter.
    /// \params portName the name of the output port
    Node::PortBaseOutRawPtr getDataGetter( const std::string& nodeName,
                                           const std::string& portName ) {
        auto node = getNode( nodeName );
        auto port = node->getOutputByName( portName );
        CORE_ASSERT( port.first.isValid(),
                     "invalid port, node: " + nodeName + " port: " + portName );

        return port.second;
    }

    /// \brief Data getter descriptor.
    /// A Data getter descriptor is composed of an output port (belonging to any node of the
    /// graph), its name and its type. Use getData on the output port to extract data from the
    /// graph. \note, a dataGetter is valid only after successful compilation of the graph.
    /// \todo find a way to test the validity of the getter (invalid if no path exists from any
    /// source port to the associated sink port)
    using DataGetterDesc = std::tuple<PortBase*, std::string, std::string>;

    bool findNode2( const Node* node ) const;

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

    /// \brief Test if a node can be added to a graph
    /// \param newNode const naked pointer to the candidate node
    /// \return true if ownership could be transferred to the graph.
    virtual bool canAdd( const Node* newNode ) const;

    /// Returns the index of the given node in the graph.
    /// if there is none, returns -1.
    /// \param name The name of the node to find.
    int findNode( const Node* node ) const;

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

    bool checkNodeValidity( const Node* nodeFrom, const Node* nodeTo ) const;
    static bool checkPortCompatibility( const Node* nodeFrom,
                                        Node::PortIndex portOutIdx,
                                        const PortBaseOut* portOut,
                                        const Node* nodeTo,
                                        Node::PortIndex portInIdx,
                                        const PortBaseIn* portIn );
    class RA_DATAFLOW_API Log
    {
      public:
        static void alreadyLinked( const Node* node, const PortBase* port );
        static void addLinkTypeMismatch( const Node* nodeFrom,
                                         Node::PortIndex portOutIdx,
                                         const PortBase* portOut,
                                         const Node* nodeTo,
                                         Node::PortIndex portInIdx,
                                         const PortBase* portIn );
        static void unableToFind( const std::string& type, const std::string& instanceName );
        static void
        badPortIdx( const std::string& type, const std::string& instanceName, Node::PortIndex idx );
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

    static_assert( std::is_same_v<T, U>, "in and out port type mismatch" );
    auto nodeFrom = outputPort->getNode();
    auto nodeTo   = inputPort->getNode();

    if ( !checkNodeValidity( nodeFrom, nodeTo ) ) { return false; }

    if ( inputPort->isLinked() ) {
        Log::alreadyLinked( nodeTo, inputPort.get() );
        return false;
    }
    inputPort->connect( outputPort.get() );

    // The state of the graph changes, set it to not ready
    needsRecompile();
    return true;
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
