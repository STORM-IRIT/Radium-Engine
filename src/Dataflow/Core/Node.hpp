#pragma once
#include "Core/Containers/VariableSet.hpp"
#include "Core/Utils/Index.hpp"
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/EditableParameter.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>
#include <Dataflow/Core/TypeDemangler.hpp>

#include <nlohmann/json.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits> // conditional, is_same
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
 * in an evaluation context (possibly empty) defined by their internal data to generate results
 * sent to their output ports.
 *
 */
class RA_DATAFLOW_API Node
{
  public:
    using PortIndex = Ra::Core::Utils::Index;

    template <typename Port>
    using PortPtr = std::shared_ptr<Port>;
    template <typename Port>
    using PortRawPtr = typename PortPtr<Port>::element_type*;

    template <typename Type>
    using PortInPtr = PortPtr<PortIn<Type>>;
    template <typename Type>
    using PortInRawPtr = typename PortInPtr<Type>::element_type*;

    template <typename Type>
    using PortOutPtr = PortPtr<PortOut<Type>>;
    template <typename Type>
    using PortOutRawPtr = typename PortOutPtr<Type>::element_type*;

    using PortBasePtr       = PortPtr<PortBase>;
    using PortBaseInPtr     = PortPtr<PortBaseIn>;
    using PortBaseOutPtr    = PortPtr<PortBaseOut>;
    using PortBaseRawPtr    = PortRawPtr<PortBase>;
    using PortBaseInRawPtr  = PortRawPtr<PortBaseIn>;
    using PortBaseOutRawPtr = PortRawPtr<PortBaseOut>;

    template <typename Port>
    using PortCollection        = std::vector<Port>;
    using PortBaseCollection    = PortCollection<PortBasePtr>;
    using PortBaseInCollection  = PortCollection<PortBaseInPtr>;
    using PortBaseOutCollection = PortCollection<PortBaseOutPtr>;

    template <typename Port>
    using IndexAndPort = std::pair<PortIndex, Port>;

    template <typename T>
    using ParamHandle = Ra::Core::VariableSet::VariableHandle<T>;

    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    /// \see https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rc-copy-virtual
    Node()                         = delete;
    Node( const Node& )            = delete;
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

    /// \brief Get a port by its name
    /// \param type either "in" or "out", the directional type of the port
    /// \param name
    /// \return the index to access the port and a raw ptr to the port.
    IndexAndPort<PortBaseRawPtr> getPortByName( const std::string& type,
                                                const std::string& name ) const;
    IndexAndPort<PortBaseInRawPtr> getInputByName( const std::string& name ) const;
    IndexAndPort<PortBaseOutRawPtr> getOutputByName( const std::string& name ) const;

    /// \brief Get a port by its index
    /// \param type either "in" or "out", the directional type of the port
    /// \param idx
    /// \return an alias pointer on the requested port if it exists, nullptr else
    PortBaseRawPtr getPortByIndex( const std::string& type, PortIndex idx ) const;
    auto getInputByIndex( PortIndex idx ) const { return getPortBase( m_inputs, idx ); }
    auto getOutputByIndex( PortIndex idx ) const { return getPortBase( m_outputs, idx ); }

    template <typename T>
    auto getInputByIndex( PortIndex idx ) const {
        return getPort<T>( m_inputs, idx );
    }

    /// \brief Gets a output typed port by its index
    /// \param idx
    /// \return
    template <typename T>
    auto getOutputByIndex( PortIndex idx ) const {
        return getPort<T>( m_outputs, idx );
    }

    /// \brief Gets the in ports of the node.
    /// Input ports are own to the node.
    const PortBaseInCollection& getInputs() const;

    /// \brief Gets the out ports of the node.
    /// Output ports are own to the node.
    const PortBaseOutCollection& getOutputs() const;

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
    /// \todo : specify the json format for nodes and what is expected from the following methods

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

    /// \brief Returns the demangled type name of the node or any human readable representation of
    /// the type name.
    /// This is a public static member each node must define to be serializable
    static const std::string& getTypename();

    inline bool isInitialized() const { return m_initialized; }
    Ra::Core::VariableSet& getParameters() { return m_parameters; }

    inline bool isOutputNode() {
        bool ret = true;
        // isOutput if none of the outputs port are connected
        for ( const auto& p : m_outputs ) {
            ret = ret && ( p->getLinkCount() == 0 );
        }
        return ret;
    }

    inline bool isInputNode() {
        bool ret = true;
        // isOutput if none of the outputs port are connected
        for ( const auto& p : m_inputs ) {
            ret = ret && p->hasDefaultValue() && !p->isLinked();
        }
        return ret;
    }

  protected:
    /// Construct the base node given its name and type
    /// \param instanceName The name of the node
    /// \param typeName The type name of the node
    Node( const std::string& instanceName, const std::string& typeName );

    ///\brief Gets the Port By Name
    ///
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports
    ///\param name The named used to add the port.
    ///\return IndexAndPort<PortRawPtr<PortType>> the index to access the port and a raw ptr to
    /// the port.
    template <typename PortType>
    IndexAndPort<PortRawPtr<PortType>>
    getPortByName( const PortCollection<PortPtr<PortType>>& ports, const std::string& name ) const {
        auto itp = std::find_if(
            ports.begin(), ports.end(), [n = name]( const auto& p ) { return p->getName() == n; } );
        PortRawPtr<PortType> fprt { nullptr };
        PortIndex portIndex;
        if ( itp != ports.cend() ) {
            fprt      = itp->get();
            portIndex = std::distance( ports.begin(), itp );
        }
        return { portIndex, fprt };
    }

    ///\brief Gets the PortBase In or Out by its index
    ///
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports
    ///\param idx
    ///\return PortRawPtr<PortType>
    template <typename PortType>
    PortRawPtr<PortType> getPortBase( const PortCollection<PortPtr<PortType>>& ports,
                                      PortIndex idx ) const {
        if ( 0 <= idx && size_t( idx ) < ports.size() ) { return ports[idx].get(); }
        return nullptr;
    }

    ///\brief Gets the PortBase In or Out by its index
    ///
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports
    ///\param idx
    ///\return PortRawPtr<PortType>
    template <typename PortType>
    PortRawPtr<PortType> getPortBaseNoCheck( const PortCollection<PortPtr<PortType>>& ports,
                                             PortIndex idx ) const {
        return ports[idx].get();
    }

    ///\brief Gets a port in a collection by its index.
    ///
    ///\tparam T The contained type.
    ///\tparam PortType PortBaseIn or PortBaseOut
    ///\param ports The port collection
    ///\param idx
    ///\return auto A raw ptr to the port typed in or out.
    template <typename T, typename PortType>
    auto getPort( const PortCollection<PortPtr<PortType>>& ports, PortIndex idx ) const {
        return static_cast<typename std::conditional<
            /*if*/ std::is_same<PortBaseIn, PortType>::value,
            /*then*/ PortInRawPtr<T>,
            /*else*/ PortOutRawPtr<T>>::type>( getPortBase( ports, idx ) );
    }

    /// internal json representation of the Node.
    /// Default implementation warn about unsupported deserialization.
    /// Effective deserialzation must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding in derived classes.
    virtual bool fromJsonInternal( const nlohmann::json& ) {
        LOG( Ra::Core::Utils::logDEBUG ) << "Unable deserializing " << getTypeName() << ".";
        return true;
    }

    /// internal json representation of the Node.
    /// Default implementation warn about unsupported serialization.
    /// Effective serialzation must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding in derived classes.
    virtual void toJsonInternal( nlohmann::json& data ) const {
        data["comment"] = std::string { "Could not serialized " } + getTypeName();
        LOG( Ra::Core::Utils::logDEBUG )
            << "Unable to save data when serializing a " << getTypeName() << ".";
    }

    /// Adds an in port to the node.
    /// This function checks if the port is an input port.
    /// \param in The in port to add.
    bool addInput( PortBaseInRawPtr in );
    PortIndex addInput( PortBaseInPtr in );
    PortIndex addOutput( PortBaseOutPtr out );
    template <typename PortType>
    PortIndex addPort( PortCollection<PortPtr<PortType>>&, PortPtr<PortType> port );

    template <typename T, typename... U>
    PortInPtr<T> addInputPort( U&&... u ) {
        auto idx = addInput( std::make_shared<PortIn<T>>( this, std::forward<U>( u )... ) );
        return getInputPort<T>( idx );
    }
    template <typename T, typename... U>
    PortOutPtr<T> addOutputPort( U&&... u ) {
        auto idx = addOutput( std::make_shared<PortOut<T>>( this, std::forward<U>( u )... ) );
        return getOutputPort<T>( idx );
    }

    template <typename T>
    PortInPtr<T> getInputPort( PortIndex idx ) {
        return std::static_pointer_cast<PortIn<T>>( m_inputs[idx] );
    }

    template <typename T>
    PortOutPtr<T> getOutputPort( PortIndex idx ) {
        return std::static_pointer_cast<PortOut<T>>( m_outputs[idx] );
    }

    PortBaseInPtr getInputPort( PortIndex idx ) { return m_inputs[idx]; }
    PortBaseOutPtr getOutputPort( PortIndex idx ) { return m_outputs[idx]; }

    ///\todo remove these it not needed by dataflow graph
    /// \brief remove the given input port from the managed input ports
    /// \param in the port to remove
    /// \return true if the port was removed (the in pointer is the set to nullptr), false else
    bool removeInput( PortBaseInRawPtr& in );
    void removeInput( PortIndex idx ) { m_inputs[idx].reset(); }

    /// Adds an out port to the node and the data associated with it.
    /// This function checks if there is no out port with the same name already associated with
    /// this node. \param out The in port to add. \param data The data associated with the port.
    template <typename T>
    void addOutput( PortOutRawPtr<T> out, T* data );

    /// \brief remove the given output port from the managed input ports
    /// \param out the port to remove
    /// \return true if the port was removed (the out pointer is the set to nullptr), false else
    bool removeOutput( PortBaseOutRawPtr& out );
    void removeOutput( PortIndex idx ) { m_outputs[idx].reset(); }

    template <typename T>
    ParamHandle<T> addParameter( const std::string& name, const T& value ) {
        return m_parameters.insertVariable<T>( name, value ).first;
    }

    template <typename T>
    bool removeParameter( const std::string& name ) {
        return m_parameters.deleteVariable( name );
    }

    template <typename T>
    bool removeParameter( ParamHandle<T>& handle ) {
        return m_parameters.deleteVariable( handle );
    }

    /// \brief get a typed reference to the editable parameter.
    /// \tparam E The type of the expected editable parameter.
    /// \param name The name of the editable parameter to get.
    /// \return the pointer to the editable parameter if any, nullptr if not.
    template <typename E>
    EditableParameter<E>* getEditableParameter( const std::string& name );

    /// \brief Flag that checks if the node is already initialized
    bool m_initialized { false };
    /// The type name of the node. Initialized once at construction
    std::string m_typeName;
    /// The instance name of the node
    std::string m_instanceName;
    /// The in ports of the node (own by the node)
    PortCollection<PortPtr<PortBaseIn>> m_inputs;
    /// The out ports of the node  (own by the node)
    PortCollection<PortPtr<PortBaseOut>> m_outputs;

    /// The editable parameters of the node
    Ra::Core::VariableSet m_parameters;

    /// Additional data on the node, added by application or gui or ...
    nlohmann::json m_extraJsonData;
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

inline void Node::init() {
    m_initialized = true;
}

inline void Node::destroy() {
    m_inputs.clear();
    m_outputs.clear();
    m_parameters.clear();
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

inline const Node::PortBaseInCollection& Node::getInputs() const {
    return m_inputs;
}

inline const Node::PortBaseOutCollection& Node::getOutputs() const {
    return m_outputs;
}

inline bool Node::operator==( const Node& o_node ) {
    return ( m_typeName == o_node.getTypeName() ) && ( m_instanceName == o_node.getInstanceName() );
}

inline bool Node::addInput( PortBaseInRawPtr in ) {
    bool found = false;
    for ( auto& input : m_inputs ) {
        if ( input->getName() == in->getName() ) { found = true; }
    }
    if ( !found ) { m_inputs.emplace_back( in ); }
    return !found;
}

template <typename PortType>
inline Node::PortIndex Node::addPort( PortCollection<PortPtr<PortType>>& ports,
                                      PortPtr<PortType> port ) {
    PortIndex idx;
    // look for a free slot
    auto it = std::find_if( ports.begin(), ports.end(), []( const auto& port ) { return !port; } );
    if ( it != ports.end() ) {
        it->swap( port );
        idx = std::distance( ports.begin(), it );
    }
    else {
        ports.push_back( std::move( port ) );
        idx = ports.size() - 1;
    }
    return idx;
}

inline Node::PortIndex Node::addInput( PortBaseInPtr in ) {
    return addPort( m_inputs, std::move( in ) );
}

inline Node::PortIndex Node::addOutput( PortBaseOutPtr out ) {
    return addPort( m_outputs, std::move( out ) );
}

inline bool Node::removeInput( PortBaseInRawPtr& in ) {
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
void Node::addOutput( PortOutRawPtr<T> out, T* data ) {
    bool found = false;
    for ( auto& output : m_outputs ) {
        if ( output->getName() == out->getName() ) { found = true; }
    }
    if ( !found ) {
        m_outputs.emplace_back( out );
        out->setData( data );
    }
}

inline bool Node::removeOutput( PortBaseOutRawPtr& out ) {
    auto outP = std::find_if(
        m_outputs.begin(), m_outputs.end(), [out]( const auto& p ) { return p.get() == out; } );
    if ( outP != m_outputs.end() ) {
        m_outputs.erase( outP );
        out = nullptr;
        return true;
    }
    return false;
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
