#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/EditableParameter.hpp>
#include <Dataflow/Core/Port.hpp>
#include <Dataflow/Core/TypeDemangler.hpp>

#include <nlohmann/json.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

namespace Ra {
namespace Dataflow {
namespace Core {

/** \brief Base abstract class for all the nodes added and used by the node system.
 * A node represent a function acting on some input data and generating some outputs.
 * To build a computation graph, nodes should be added to the graph, which is itself a node
 * (\see Ra::Dataflow::Core::DataflowGraph) and linked together through their input/output port.
 *
 * Nodes computes their function using the input data collecting from the input ports,
 * in an evaluation context (possibly empty) defined byt their internal data to generate results
 * sent to their output ports.
 *
 */
class RA_DATAFLOW_API Node
{
  public:
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    /// \see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    Node()              = delete;
    Node( const Node& ) = delete;
    Node& operator=( const Node& ) = delete;
    /// @}

    /// \brief make Node a base abstract class
    virtual ~Node() = default;

    /// \brief Two nodes are considered equal if there type and instance names are the same.
    bool operator==( const Node& o_node );

    /// \name Function execution control
    /// @{
    /// \brief Initializes the node content
    /// The init() function should be called once at the beginning of the lifetime of the node by
    /// the owner of the node (the graph which contains the node).
    /// Its goal is to initialize the node's internal data if any.
    /// The base version do nothing.
    virtual void init();

    /// \brief Compile the node to check its validity
    /// Only nodes defining a full computation graph will need to override this method.
    /// The base version do nothing.
    /// \return the compilation status
    virtual bool compile();

    /// \brief Executes the node.
    /// Execute the node function on the input ports (to be fetched) and write the results to the
    /// output ports.
    /// \return the execution status.
    virtual bool execute() = 0;

    /// \brief delete the node content
    /// The destroy() function is called once at the end of the lifetime of the node.
    /// Its goal is to free the internal data that have been allocated.
    virtual void destroy();
    /// @}

    /// \name Control the interfaces of the nodes (inputs, outputs, internal data, ...)
    /// @{

    /// \brief Get an input port by its name
    /// \param type either "in" or "out", the directional type of the port
    /// \param name
    /// \return an alias pointer on the requested port if it exists, nullptr else
    PortBase* getPortByName( const std::string& type, const std::string& name ) const;

    /// \brief Get an input port by its index
    /// \param type either "in" or "out", the directional type of the port
    /// \param idx
    /// \return an alias pointer on the requested port if it exists, nullptr else
    PortBase* getPortByIndex( const std::string& type, int idx ) const;

    /// \brief Gets the in ports of the node.
    /// Input ports are own to the node.
    const std::vector<std::unique_ptr<PortBase>>& getInputs() const;

    /// \brief Gets the out ports of the node.
    /// Output ports are own to the node.
    const std::vector<std::unique_ptr<PortBase>>& getOutputs() const;

    /// \brief Build the interface ports of the node
    /// Derived node can override the default implementation that build an interface port for each
    /// input or output port (e.g. if several inputs have the same type T, make an interface that is
    /// a vector of T*)
    virtual const std::vector<PortBase*>& buildInterfaces( Node* parent );

    /// \brief Get the interface ports of the node
    const std::vector<PortBase*>& getInterfaces() const;

    /// \brief Gets the editable parameters of the node.
    /// used only by the node editor gui to build the editon widget
    const std::vector<std::unique_ptr<EditableParameterBase>>& getEditableParameters();
    /// @}

    /// \name Identification methods
    /// @{
    /// \brief Gets the type name of the node.
    const std::string& getTypeName() const;

    /// \brief Gets the instance name of the node.
    const std::string& getInstanceName() const;

    /// \brief Sets the instance name (rename) the node
    void setInstanceName( const std::string& newName );

    /// @}

    /// \name Serialization of a node
    /// @{
    /// TODO : specify the json format for nodes and what is expected from the following ethods

    /// \brief serialize the content of the node.
    /// Fill the given json object with the json representation of the concrete node.
    void toJson( nlohmann::json& data ) const;

    /// \brief unserialized the content of the node.
    /// Fill the node from its json representation
    bool fromJson( const nlohmann::json& data );

    /// \brief Add a metadata to the node to store application specific information.
    /// used, e.g. by the node editor gui to save node position in the graphical canvas.
    void addJsonMetaData( const nlohmann::json& data );

    /// \brief Give access to extra json data stored on the node.
    const nlohmann::json& getJsonMetaData();
    /// @}

    /// \brief Flag that checks if the node is already initialized
    bool m_initialized { false };

    /// \brief Sets the filesystem (real or virtual) location for the node resources
    inline void setResourcesDir( std::string resourcesRootDir );

  protected:
    /// Construct the base node given its name and type
    /// \param instanceName The name of the node
    /// \param typeName The type name of the node
    Node( const std::string& instanceName, const std::string& typeName );

    /// internal json representation of the Node.
    /// Must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding when implementing
    /// this method.
    virtual bool fromJsonInternal( const nlohmann::json& ) = 0;

    /// internal json representation of the Node.
    /// Must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding when implementing
    /// this method.
    virtual void toJsonInternal( nlohmann::json& ) const = 0;

    /// Adds an in port to the node.
    /// This function checks if the port is an input port, then if there is no in port with the same
    /// name already associated with this node.
    /// \param in The in port to add.
    bool addInput( PortBase* in );

    /// \brief remove the given input port from the managed input ports
    /// \param in the port to remove
    /// \return true if the port was removed (the in pointer is the set to nullptr), false else
    bool removeInput( PortBase*& in );

    /// Adds an out port to the node and the data associated with it.
    /// This function checks if there is no out port with the same name already associated with this
    /// node.
    /// \param out The in port to add.
    /// \param data The data associated with the port.
    template <typename T>
    void addOutput( PortOut<T>* out, T* data );

    /// \brief remove the given output port from the managed input ports
    /// \param out the port to remove
    /// \return true if the port was removed (the out pointer is the set to nullptr), false else
    bool removeOutput( PortBase*& out );

    /// \brief Adds an editable parameter to the node if it does not already exist.
    /// \note the node will take ownership of the editable object.
    /// \param editableParameter The editable parameter to add.
    bool addEditableParameter( EditableParameterBase* editableParameter );

    /// Remove an editable parameter to the node if it does exist.
    /// \param name The name of the editable parameter to remove.
    /// \return true if the editable parameter is found and removed.
    bool removeEditableParameter( const std::string& name );

    /// \brief get a typed reference to the editable parameter.
    /// \tparam E The type of the expected editable parameter.
    /// \param name The name of the editable parameter to get.
    /// \return the pointer to the editable parameter if any, nullptr if not.
    template <typename E>
    EditableParameter<E>* getEditableParameter( const std::string& name );

    /// The type name of the node. Initialized once at construction
    std::string m_typeName;
    /// The instance name of the node
    std::string m_instanceName;
    /// The in ports of the node (own by the node)
    std::vector<std::unique_ptr<PortBase>> m_inputs;
    /// The out ports of the node  (own by the node)
    std::vector<std::unique_ptr<PortBase>> m_outputs;
    /// The reflected ports of the node if it is only a source or sink node.
    /// This stores only aliases as interface ports will belong to the parent
    /// node (i.e. the graph this node belongs to)
    std::vector<PortBase*> m_interface;

    /// The editable parameters of the node
    std::vector<std::unique_ptr<EditableParameterBase>> m_editableParameters;

    /// Additional data on the node, added by application or gui or ...
    nlohmann::json m_extraJsonData;

  public:
    /// \brief Returns the demangled type name of the node or any human readable representation of
    /// the type name.
    /// This is a public static member each node must define to be serializable
    static const std::string& getTypename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

inline void Node::init() {
    m_initialized = true;
}

inline void Node::destroy() {
    m_interface.clear();
    m_inputs.clear();
    m_outputs.clear();
    m_editableParameters.clear();
}

inline const nlohmann::json& Node::getJsonMetaData() {
    return m_extraJsonData;
}

inline const std::string& Node::getTypeName() const {
    return m_typeName;
}

inline const std::string& Node::getInstanceName() const {
    return m_instanceName;
}

inline void Node::setInstanceName( const std::string& newName ) {
    m_instanceName = newName;
}

inline const std::vector<std::unique_ptr<PortBase>>& Node::getInputs() const {
    return m_inputs;
}

inline const std::vector<std::unique_ptr<PortBase>>& Node::getOutputs() const {
    return m_outputs;
}

inline const std::vector<PortBase*>& Node::buildInterfaces( Node* parent ) {
    m_interface.clear();
    m_interface.shrink_to_fit();
    const std::vector<std::unique_ptr<PortBase>>* readFrom =
        m_inputs.empty() ? &m_outputs : &m_inputs;
    m_interface.reserve( readFrom->size() );
    for ( const auto& p : *readFrom ) {
        m_interface.emplace_back( p->reflect( parent, getInstanceName() + '_' + p->getName() ) );
    }
    return m_interface;
}

inline const std::vector<PortBase*>& Node::getInterfaces() const {
    return m_interface;
}

inline const std::vector<std::unique_ptr<EditableParameterBase>>& Node::getEditableParameters() {
    return m_editableParameters;
}

inline bool Node::operator==( const Node& o_node ) {
    return ( m_typeName == o_node.getTypeName() ) && ( m_instanceName == o_node.getInstanceName() );
}

inline bool Node::addInput( PortBase* in ) {
    if ( !in->is_input() ) { return false; }
    bool found = false;
    for ( auto& input : m_inputs ) {
        if ( input->getName() == in->getName() ) { found = true; }
    }
    if ( !found ) { m_inputs.emplace_back( in ); }
    return !found;
}

inline bool Node::removeInput( PortBase*& in ) {
    auto itP = std::find_if(
        m_inputs.begin(), m_inputs.end(), [in]( const auto& p ) { return p.get() == in; } );
    if ( itP != m_inputs.end() ) {
        m_inputs.erase( itP );
        in = nullptr;
        return true;
    }
    return false;
}

template <typename T>
void Node::addOutput( PortOut<T>* out, T* data ) {
    bool found = false;
    for ( auto& output : m_outputs ) {
        if ( output->getName() == out->getName() ) { found = true; }
    }
    if ( !found ) {
        m_outputs.emplace_back( out );
        out->setData( data );
    }
}

inline bool Node::removeOutput( PortBase*& out ) {
    auto outP = std::find_if(
        m_outputs.begin(), m_outputs.end(), [out]( const auto& p ) { return p.get() == out; } );
    if ( outP != m_outputs.end() ) {
        m_outputs.erase( outP );
        out = nullptr;
        return true;
    }
    return false;
}

inline bool Node::addEditableParameter( EditableParameterBase* editableParameter ) {
    bool found = false;
    for ( auto& edit : m_editableParameters ) {
        if ( edit.get()->getName() == editableParameter->getName() ) { found = true; }
    }
    if ( !found ) { m_editableParameters.emplace_back( editableParameter ); }
    return !found;
}

inline bool Node::removeEditableParameter( const std::string& name ) {
    bool found = false;
    auto it    = m_editableParameters.begin();
    while ( it != m_editableParameters.end() ) {
        if ( ( *it ).get()->getName() == name ) {
            m_editableParameters.erase( it );
            found = true;
            break;
        }
        ++it;
    }
    return found;
}

template <typename E>
inline EditableParameter<E>* Node::getEditableParameter( const std::string& name ) {
    auto it = m_editableParameters.begin();
    while ( it != m_editableParameters.end() ) {
        if ( ( *it ).get()->getName() == name ) {
            auto p = dynamic_cast<EditableParameter<E>*>( ( *it ).get() );
            if ( p != nullptr ) { return *p; }
        }
        ++it;
    }
    return nullptr;
}

inline const std::string& Node::getTypename() {
    static std::string demangledTypeName { "Abstract Node" };
    return demangledTypeName;
}

inline bool Node::compile() {
    return true;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
