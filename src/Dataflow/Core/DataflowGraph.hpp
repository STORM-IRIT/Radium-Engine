#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Enumerator.hpp>
#include <Dataflow/Core/Node.hpp>
#include <Dataflow/Core/NodeFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

/**
 *  \todo : clarify what is a source and how to use it
 *      if sources must be used through interface port only, delete the set_data on all sources
 *  \todo Are node aliases a should-have (to make editing more user friendly)???
 */

/**
 * \brief Represent a set of connected nodes that define a computational graph
 * Ownership of nodes is given to the graph at construction time.
 * \todo make a "graph embedding node" that allow to seemlesly integrate a graph as a node in
 * another graph
 *      --> Edition of a graph will allow loading and saving to a file directly
 *      --> Edition of an embeded graph will defer loading and saving to the parent graph
 *          --> for this, need to decide if a subgraph is stored in the json of its parent or in a
 * separate file
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

    /// Set the factory set to use when loading a graph
    /// This function replace the existing factoryset if any
    void setNodeFactories( std::shared_ptr<NodeFactorySet> factories );

    /// get the node factory set associated with from the graph.
    /// returns nullptr if no factoryset is associated with the graph
    std::shared_ptr<NodeFactorySet> getNodeFactories() const;

    /// Add a factory to the factoryset of the graph.
    /// Creates the factoryset if it does not exists
    void addFactory( const std::string& name, std::shared_ptr<NodeFactory> f );

    /// Add a factory to the factoryset of the graph.
    /// Creates the factoryset if it does not exists
    void addFactory( std::shared_ptr<NodeFactory> f );

    /// Loads nodes and links from a JSON file.
    /// \param jsonFilePath The path to the JSON file.
    bool loadFromJson( const std::string& jsonFilePath );

    /// Saves nodes and links to a JSON file.
    /// \param jsonFilePath The path to the JSON file.
    void saveToJson( const std::string& jsonFilePath );

    /// \brief Test if a node can be added to a graph
    /// \param newNode const naked pointer to the candidate node
    /// \return true if ownership could be transferred to the graph.
    virtual bool canAdd( const Node* newNode ) const;
    /// Adds a node to the render graph. Adds interface ports to the node newNode and the
    /// corresponding input and output ports to the graph.
    /// \param newNode The node to add to the render graph.
    /// ownership of the node is transfered to the graph
    /// \todo move ine unique_ptr and return an optional with the raw pointer
    virtual bool addNode( Node* newNode );
    /// Removes a node from the render graph. Removes input and output ports of the graph
    /// corresponding to interface ports of the node.
    /// \param node The node to remove from the render graph.
    /// \return true if the node was removed and the given pointer is set to nullptr, false else
    virtual bool removeNode( Node*& node );
    /// Connects two nodes of the render graph.
    /// The two nodes must already be in the render graph (with the addNode(Node* newNode)
    /// function), the first node's in port must be free and the connected in port and out port must
    /// have the same type of data.
    /// \param nodeFrom The node that contains the out port.
    /// \param nodeFromOutputName The name of the out port in nodeFrom.
    /// \param nodeTo The node that contains the in port.
    /// \param nodeToInputName The name of the in port in nodeTo.
    bool addLink( Node* nodeFrom,
                  const std::string& nodeFromOutputName,
                  Node* nodeTo,
                  const std::string& nodeToInputName );
    /// Removes the link connected to this node's input port
    bool removeLink( Node* node, const std::string& nodeInputName );

    /// Gets the nodes
    const std::vector<std::unique_ptr<Node>>* getNodes() const;

    /// Gets a specific node according to its instance name.
    /// \param instanceNameNode The instance name of the node.
    Node* getNode( const std::string& instanceNameNode ) const;

    /// Gets the nodes ordered by level (after compilation)
    const std::vector<std::vector<Node*>>* getNodesByLevel() const;

    /// Compile the render graph to check its validity and simplify it.
    /// The compilation has multiple goals:
    /// - Remove the nodes that have no direct or indirect connections to sink nodes
    /// - Order the nodes by level according to their dependencies
    /// - Check if every mandatory port is linked
    bool compile() override;

    /// Gets the number of nodes
    size_t getNodesCount() const;

    /// Deletes all nodes from the render graph.
    virtual void clearNodes();

    /// Flag set after successful compilation indicating graph is ready to be executed
    /// This flag is reset as soon as the graph is modified.
    bool m_ready { false };

    /// Flag that indicates if the graph should be saved to a file
    /// This flag is useless outside an load/edit/save scenario
    bool m_shouldBeSaved { false };

    /// \brief Creates an output port connected to the named input port of the graph.
    /// Return the connected output port if success, sharing the ownership with the caller.
    /// Allows to set data to the graph from the caller .
    /// \note As ownership is shared with the caller, the graph must survive the returned
    /// pointer to be able to use the dataSetter..
    /// \params portName The name of the input port of the graph
    std::shared_ptr<PortBase> getDataSetter( std::string portName );

    /// \brief disconnect the data setting port from its inputs.
    bool releaseDataSetter( std::string portName );
    /// \brief connect the data setting port from its inputs.
    bool activateDataSetter( std::string portName );

    /// Returns an alias to the named output port of the graph.
    /// Allows to get the data stored at this port after the execution of the graph.
    /// \note ownership is left to the graph. The graph must survive the returned
    /// pointer to be able to use the dataGetter..
    /// \params portName the name of the output port
    PortBase* getDataGetter( std::string portName );

    using DataSetterDesc = std::tuple<std::shared_ptr<PortBase>, std::string, std::string>;
    using DataGetterDesc = std::tuple<PortBase*, std::string, std::string>;

    /// Creates a vector that stores all the DataSetters (\see getDataSetter) of the graph.
    /// A tuple is composed of an output port connected to an input port of the graph, its name its
    /// type. \note If called multiple times for the same port, only the last returned result is
    /// usable.
    /// TODO : Verify why, when listing the data setters, they are connected ...
    std::vector<DataSetterDesc> getAllDataSetters();

    /// Creates a vector that stores all the DataGetters (\see getDataGetter) of the graph.
    /// A tuple is composed of an output port belonging to the graph, its name its type.
    std::vector<DataGetterDesc> getAllDataGetters();

  protected:
    /** Allow derived class to construct the graph with their own static type
     */
    DataflowGraph( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& data ) override;
    void toJsonInternal( nlohmann::json& ) const override;

  private:
    /// The node factory to use for loading
    std::shared_ptr<NodeFactorySet> m_factories;
    /// The unordered list of nodes.
    std::vector<std::unique_ptr<Node>> m_nodes;
    // Internal node levels representation
    /// The list of nodes ordered by levels.
    /// Two nodes at the same level have no dependency between them.
    std::vector<std::vector<Node*>> m_nodesByLevel;

    // Internal helper functions
    /// Internal compilation function that allows to go back in the render graph while filling an
    /// information map.
    /// \param current The current node.
    /// \param infoNodes The map that contains information about nodes.
    void backtrackGraph( Node* current,
                         std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes );
    /// Internal compilation function that allows to go through the render graph, using an
    /// information map.
    /// \param current The current node.
    /// \param infoNodes The map that contains information about nodes.
    int goThroughGraph( Node* current,
                        std::unordered_map<Node*, std::pair<int, std::vector<Node*>>>& infoNodes );
    /// Returns the index of the given node in the graph.
    /// if there is none, returns -1.
    /// \param name The name of the node to find.
    int findNode( const Node* node ) const;

    /// Data setters management : used to pass parameter to the graph when the graph is not embedded
    /// into another graph (inputs are here for this case).
    /// A dataSetter is an outputPort, associated to an input port of the graph.
    /// The connection between these ports can be activated/deactivated using
    /// activateDataSetter/releaseDataSetter
    using DataSetter = std::pair<DataSetterDesc, PortBase*>;
    std::map<std::string, DataSetter> m_dataSetters;
    /// \brief Adds an input port to the graph and associate it with a dataSetter.
    /// This port is aliased as an interface port in a source node of the graph.
    /// This function checks if there is no input port with the same name already
    /// associated with the graph.
    bool addSetter( PortBase* in );

    /// \brief Adds an out port for a Graph and register it as a dataGetter.
    /// This port is aliased as an interface port in a sink node of the graph.
    /// This function checks if there is no out port with the same name already
    /// associated with the graph.
    /// \param out The port to add.
    bool addGetter( PortBase* out );

  public:
    static const std::string& getTypename();

    /**
     * \brief Load a graph from the given file.
     * \param filename
     * Any type of graph that inherits from DataflowGraph can be loaded by this function as soon as
     * the appropriate constructor is registered in the node factory.
     * \return The loaded graph, as a DataFlowGraph pointer to be downcast to the correct type
     */
    static DataflowGraph* loadGraphFromJsonFile( const std::string& filename );

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

  private:
    bool m_nodesAndLinksProtected { false };
};

} // namespace Core
} // namespace Dataflow
} // namespace Ra

#include <Dataflow/Core/DataflowGraph.inl>
