#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Core/Containers/VariableSet.hpp>
#include <Core/Utils/Index.hpp>
#include <Dataflow/Core/PortFactory.hpp>
#include <Dataflow/Core/PortIn.hpp>
#include <Dataflow/Core/PortOut.hpp>

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
 * Derived class must implement bool execute() and static const std::string & typename()
 *
 * static const std::string& typename() returns the demangled type name of the node or any human
 * readable representation of the type name. This is a public static member each node concrete class
 * must define to be serializable. Since we want to manipulate Node*, CRTP is not an option here.
 */
class RA_DATAFLOW_CORE_API Node
{
  public:
    using PortIndex = Ra::Core::Utils::Index;

    template <typename Port>
    using PortCollection = std::vector<Port>;

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

    using PortBasePtr        = PortPtr<PortBase>;
    using PortBaseRawPtr     = PortRawPtr<PortBase>;
    using PortBaseCollection = PortCollection<PortBasePtr>;

    using PortBaseInPtr        = PortPtr<PortBaseIn>;
    using PortBaseInRawPtr     = PortRawPtr<PortBaseIn>;
    using PortBaseInCollection = PortCollection<PortBaseInPtr>;

    using PortBaseOutPtr        = PortPtr<PortBaseOut>;
    using PortBaseOutRawPtr     = PortRawPtr<PortBaseOut>;
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

    /// \brief Two nodes are considered equal if there model and instance names are the same.
    bool operator==( const Node& node );

    /// \name Function execution control
    /// @{
    /** \brief Initializes the node content.
     * The init() function should be called once at the beginning of the lifetime of the node by
     * the owner of the node (the graph which contains the node).
     * Its goal is to initialize the node's internal data if any.
     * The default do nothing.
     */
    virtual void init();

    /** \brief Compile the node to check its validity.
     * Only nodes defining a full computation graph will need to override this method.
     * The base version do nothing.
     * \return the compilation status
     */
    virtual bool compile();

    /** \brief Executes the node.
     * Execute the node function on the input ports (to be fetched) and write the results to the
     * output ports.
     * \return the execution status.
     */
    virtual bool execute() = 0;

    /** \brief delete the node content
     * The destroy() function is called once at the end of the lifetime of the node.
     * Its goal is to free the internal data that have been allocated.
     */
    virtual void destroy();
    /// @}

    /// \name Control the interfaces of the nodes (inputs, outputs, internal data, ...)
    /// @{

    /** \brief Get a port by its name
     * \param type either "in" or "out", the directional type of the port
     * \param name
     * \return the index to access the port and a raw ptr to the port.
     */
    IndexAndPort<PortBaseRawPtr> port_by_name( const std::string& type,
                                               const std::string& name ) const;

    /// Convenience alias to port_by_name("in", name)
    IndexAndPort<PortBaseInRawPtr> input_by_name( const std::string& name ) const;
    /// Convenience alias to port_by_name("out", name)
    IndexAndPort<PortBaseOutRawPtr> output_by_name( const std::string& name ) const;

    /**
     * \brief Get a port by its index
     * \param type either "in" or "out", the directional type of the port
     * \param index Index of the port \in [0 get(Inputs|Output).size()[
     * \return a raw pointer on the requested port if it exists, nullptr else
     */
    PortBaseRawPtr port_by_index( const std::string& type, PortIndex index ) const;
    /// Convenience alias to port_by_index("in", index)
    auto input_by_index( PortIndex index ) const { return getPortBase( m_inputs, index ); }
    /// Convenience alias to port_by_index("out", index)
    auto output_by_index( PortIndex index ) const { return getPortBase( m_outputs, index ); }
    /// Convenience alias with typed port
    template <typename T>
    auto input_by_index( PortIndex idx ) const {
        return getPort<T>( m_inputs, idx );
    }
    /// Convenience alias with typed port
    template <typename T>
    auto output_by_index( PortIndex idx ) const {
        return getPort<T>( m_outputs, idx );
    }

    /**
     * \brief Gets the in ports of the node.
     * Input ports are own by the node.
     */
    const PortBaseInCollection& inputs() const;

    /**
     * \brief Gets the out ports of the node.
     * Output ports are own by the node.
     */
    const PortBaseOutCollection& outputs() const;

    /// \name Identification methods
    /// @{
    /// \brief Gets the model (type/class) name of the node.
    const std::string& model_name() const;
    const std::string& display_name() const { return m_display_name; }
    void set_display_name( const std::string& name ) { m_display_name = name; }

    /// \brief Gets the instance name of the node.
    const std::string& instance_name() const;

    /// \brief Sets the instance name (rename) the node
    void set_instance_name( const std::string& newName );

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

    /**
     *\brief Add a metadata to the node to store application specific information.
     *
     * Merge/replace node's metadata using \p data.
     * Used, e.g. by the node editor gui to save node position in the graphical canvas.
     * \param data a json object containing metadata.
     */
    void add_metadata( const nlohmann::json& data );

    /// \brief Give access to extra json data stored on the node.
    const nlohmann::json& metadata();
    /// @}

    inline bool is_initialized() const { return m_initialized; }

    /// \brief Return node's parameters
    Ra::Core::VariableSet& parameters() { return m_parameters; }

    /// \brief Return a variable set of input ports default value reference, if any.
    Ra::Core::VariableSet& input_variables();

    /// \brief Is output if none of the output ports is linked.
    inline bool is_output();

    /// \brief is Input if all input ports have default values and not linked.
    inline bool is_input();

  protected:
    /** \brief
     * Construct the base node given its name and type.
     *
     * \param instanceName The name of the node, unique in a graph
     * \param typeName The type name of the node, from static typename() concrete node class.
     */
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
        auto itr = std::find_if(
            ports.begin(), ports.end(), [n = name]( const auto& p ) { return p->getName() == n; } );
        PortRawPtr<PortType> port { nullptr };
        PortIndex portIndex;
        if ( itr != ports.cend() ) {
            port      = itr->get();
            portIndex = std::distance( ports.begin(), itr );
        }
        return { portIndex, port };
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
    virtual bool fromJsonInternal( const nlohmann::json& data ) {
        LOG( Ra::Core::Utils::logDEBUG )
            << "default deserialization for " << instance_name() + " " + model_name() << ".";
        if ( const auto& ports = data.find( "inputs" ); ports != data.end() ) {
            for ( const auto& port : *ports ) {
                int index = port["port_index"];
                m_inputs[index]->from_json( port );
            }
        }
        if ( const auto& ports = data.find( "outputs" ); ports != data.end() ) {
            for ( const auto& port : *ports ) {
                int index = port["port_index"];
                m_outputs[index]->from_json( port );
            }
        }
        return true;
    }

    /// internal json representation of the Node.
    /// Default implementation warn about unsupported serialization.
    /// Effective serialzation must be implemented by inheriting classes.
    /// Be careful with template specialization and function member overriding in derived classes.
    virtual void toJsonInternal( nlohmann::json& data ) const {
        std::string message =
            std::string { "default serialization for " } + instance_name() + " " + model_name();

        for ( size_t i = 0; i < m_inputs.size(); ++i ) {
            const auto& p = m_inputs[i];
            nlohmann::json port;
            p->to_json( port );
            port["port_index"] = i;
            port["type"]       = Ra::Core::Utils::simplifiedDemangledType( p->getType() );
            data["inputs"].push_back( port );
        }
        for ( size_t i = 0; i < m_outputs.size(); ++i ) {
            const auto& p = m_outputs[i];
            nlohmann::json port;
            p->to_json( port );
            port["port_index"] = i;
            port["type"]       = Ra::Core::Utils::simplifiedDemangledType( p->getType() );
            data["outputs"].push_back( port );
        }
        LOG( Ra::Core::Utils::logDEBUG ) << message;
    }

    /// Adds an in port to the node.
    /// If an input porst with the same name, do not insert and return false.
    /// \param in The in port to add.
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

    /// \todo remove these if not needed by dataflow graph
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

    /// \brief Flag that checks if the node is already initialized
    bool m_initialized { false };
    /// The type name of the node. Initialized once at construction
    std::string m_modelName;
    /// The instance name of the node
    std::string m_instanceName;
    /// Node's name if needed for display
    std::string m_display_name { "" };

    /// The in ports of the node (own by the node)
    PortCollection<PortPtr<PortBaseIn>> m_inputs;
    /// The out ports of the node  (own by the node)
    PortCollection<PortPtr<PortBaseOut>> m_outputs;

    /// The editable parameters of the node
    Ra::Core::VariableSet m_parameters;
    Ra::Core::VariableSet m_input_variables;

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
    m_input_variables.clear();
}

inline const nlohmann::json& Node::metadata() {
    return m_extraJsonData;
}

inline const std::string& Node::model_name() const {
    return m_modelName;
}

inline const std::string& Node::instance_name() const {
    return m_instanceName;
}

inline void Node::set_instance_name( const std::string& newName ) {
    m_instanceName = newName;
}

inline const Node::PortBaseInCollection& Node::inputs() const {
    return m_inputs;
}

inline const Node::PortBaseOutCollection& Node::outputs() const {
    return m_outputs;
}

inline bool Node::operator==( const Node& node ) {
    return ( m_modelName == node.model_name() ) && ( m_instanceName == node.instance_name() );
}

template <typename PortType>
inline Node::PortIndex Node::addPort( PortCollection<PortPtr<PortType>>& ports,
                                      PortPtr<PortType> port ) {
    PortIndex idx;
    // look for a free slot
    auto it = std::find_if( ports.begin(), ports.end(), []( const auto& p ) { return !p; } );
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

inline bool Node::compile() {
    return true;
}

/// \brief Return a variable set of input ports default value reference, if any.
inline Ra::Core::VariableSet& Node::input_variables() {
    m_input_variables.clear();
    for ( const auto& p : m_inputs ) {
        if ( p->hasDefaultValue() ) p->insert( m_input_variables );
    }

    return m_input_variables;
}

/// \brief Is output if none of the output ports is linked.
inline bool Node::is_output() {
    bool ret = true;
    for ( const auto& p : m_outputs ) {
        ret = ret && ( p->getLinkCount() == 0 );
    }
    return ret;
}

/// \brief is Input if all input ports have default values and not linked.
inline bool Node::is_input() {
    bool ret = true;
    //
    for ( const auto& p : m_inputs ) {
        ret = ret && p->hasDefaultValue() && !p->isLinked();
    }
    return ret;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
