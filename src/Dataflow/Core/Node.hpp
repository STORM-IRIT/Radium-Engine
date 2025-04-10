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

/**
 * \brief Base abstract class for all the nodes added and used by the node system.
 *
 * A node represent a function acting on some input data and generating some outputs.
 * To build a computation graph, nodes should be added to the graph, which is itself a node
 * (\see Ra::Dataflow::Core::DataflowGraph) and linked together through their input/output port.
 *
 * Nodes computes their function using the input data collecting from the input ports,
 * in an evaluation context (possibly empty) defined by their internal data to generate results
 * sent to their output ports.
 *
 * Derived class must implement bool execute() and static const std::string & node_typename()
 *
 * static const std::string& node_typename() returns the demangled type name of the node or any
 * human readable representation of the type name. This is a public static member each node concrete
 * class must define to be serializable. Since we want to manipulate Node*, CRTP is not an option
 * here.
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
    using PortRawPtr = Port*;

    template <typename Type>
    using PortInPtr = PortPtr<PortIn<Type>>;
    template <typename Type>
    using PortInRawPtr = PortIn<Type>*;

    template <typename Type>
    using PortOutPtr = PortPtr<PortOut<Type>>;
    template <typename Type>
    using PortOutRawPtr = PortOut<Type>*;

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
    /**
     * \brief Initializes the node content.
     *
     * The init() function should be called once at the beginning of the lifetime of the node by
     * the owner of the node (the graph which contains the node).
     * Its goal is to initialize the node's internal data if any.
     * The default do nothing.
     */
    virtual void init();

    /**
     *\brief Compile the node to check its validity.
     *
     * Only nodes defining a full computation graph will need to override this method.
     * The base version do nothing.
     * \return the compilation status
     */
    virtual bool compile();

    /**
     * \brief Executes the node.
     *
     * Execute the node function on the input ports (to be fetched) and write the results to the
     * output ports.
     * \return the execution status.
     */
    virtual bool execute() = 0;

    /**
     * \brief Delete the node's content.
     *
     * The destroy() function is called once at the end of the lifetime of the node.
     * Its goal is to free the internal data that have been allocated.
     */
    virtual void destroy();
    /// @}

    /// \name Control the interfaces of the nodes (inputs, outputs, internal data, ...)
    /// @{

    /**
     * \brief Get a port by its name
     *
     * \param type either "in" or "out", the directional type of the port
     * \param name
     * \return the index to access the port and a raw ptr to the port.
     */
    auto port_by_name( const std::string& type, const std::string& name ) const
        -> IndexAndPort<PortBaseRawPtr>;

    /// Convenience alias to port_by_name("in", name)
    IndexAndPort<PortBaseInRawPtr> input_by_name( const std::string& name ) const;
    /// Convenience alias to port_by_name("out", name)
    IndexAndPort<PortBaseOutRawPtr> output_by_name( const std::string& name ) const;

    /**
     * \brief Get a port by its index
     *
     * \param type either "in" or "out", the directional type of the port
     * \param index Index of the port \in [0 get(Inputs|Output).size()[
     * \return a raw pointer on the requested port if it exists, nullptr else
     */
    PortBaseRawPtr port_by_index( const std::string& type, PortIndex index ) const;
    /// Convenience alias to port_by_index("in", index)
    auto input_by_index( PortIndex index ) const { return port_base( m_inputs, index ); }
    /// Convenience alias to port_by_index("out", index)
    auto output_by_index( PortIndex index ) const { return port_base( m_outputs, index ); }
    /// Convenience alias with typed port
    template <typename T>
    auto input_by_index( PortIndex index ) const {
        return port<T>( m_inputs, index );
    }
    /// Convenience alias with typed port
    template <typename T>
    auto output_by_index( PortIndex index ) const {
        return port<T>( m_outputs, index );
    }

    /**
     * \brief Gets the in ports of the node.
     *
     * Input ports are own by the node.
     */
    const PortBaseInCollection& inputs() const;

    /**
     * \brief Gets the out ports of the node.
     *
     * Output ports are own by the node.
     */
    const PortBaseOutCollection& outputs() const;

    /// \name Identification methods
    /// @{

    /// \brief Gets the model (type/class) name of the node.
    const std::string& model_name() const;
    /// \brief Gets the display name of the node (e.g. for gui), no need to be unique in a graph.
    const std::string& display_name() const { return m_display_name; }
    /// \brief Set the display name.
    void set_display_name( const std::string& name ) { m_display_name = name; }

    /// \brief Gets the instance name of the node.
    const std::string& instance_name() const;

    /// \brief Sets the instance name the node (unused?) instance name must be unique in a graph.
    void set_instance_name( const std::string& name );

    /// @}

    /// \name Serialization of a node
    /// @{
    /// \todo specify the json format for nodes and what is expected from the following methods
    /// \todo use standardize json serialization

    /**
     * \brief serialize the content of the node.
     *
     * Fill the given json object with the json representation of the concrete node.
     */
    void toJson( nlohmann::json& data ) const;

    /**
     * \brief unserialized the content of the node.
     *
     * Fill the node from its json representation.
     */
    bool fromJson( const nlohmann::json& data );

    /// @}
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

    inline bool is_initialized() const { return m_initialized; }

    /// \brief Return node's parameters
    Ra::Core::VariableSet& parameters() { return m_parameters; }

    /// \brief Return a variable set of input ports default value reference, if any.
    Ra::Core::VariableSet& input_variables();

    /// \brief Node is output if none of the output ports is linked.
    inline bool is_output();

    /// \brief Node is input if all input ports have default values and not linked.
    inline bool is_input();

  protected:
    /**
     * \brief Construct the base node given its name and type.
     *
     * \param instanceName The name of the node, unique in a graph
     * \param typeName The type name of the node, from static typename() concrete node class.
     */
    Node( const std::string& instance, const std::string& typeName );

    /**
     * \brief Gets the Port By Name
     *
     * \tparam PortType PortBaseIn or PortBaseOut
     * \param ports Port collection to search in.
     * \param name The named used to add the port.
     * \return IndexAndPort<PortRawPtr<PortType>> the index to access the port and a raw ptr to
     * the port.
     */
    template <typename PortType>
    IndexAndPort<PortRawPtr<PortType>> port_by_name( const PortCollection<PortPtr<PortType>>& ports,
                                                     const std::string& name ) const;
    /**
     * \brief Gets the PortBase In or Out by its index.
     *
     * \tparam PortType PortBaseIn or PortBaseOut
     * \param ports
     * \param idx
     * \return PortRawPtr<PortType>
     */
    template <typename PortType>
    auto port_base( const PortCollection<PortPtr<PortType>>& ports, PortIndex idx ) const
        -> PortRawPtr<PortType>;
    /**
     * \brief Gets a port in a collection by its index.
     *
     * \tparam T The contained type.
     * \tparam PortType PortBaseIn or PortBaseOut
     * \param ports The port collection
     * \param idx
     * \return auto A raw ptr to the port typed in or out.
     */
    template <typename T, typename PortType>
    auto port( const PortCollection<PortPtr<PortType>>& ports, PortIndex index ) const {
        return static_cast<typename std::conditional<
            /*if*/ std::is_same<PortBaseIn, PortType>::value,
            /*then*/ PortInRawPtr<T>,
            /*else*/ PortOutRawPtr<T>>::type>( port_base( ports, index ) );
    }
    /**
     *  \brief Internal json representation of the Node.
     *
     *  Default implementation warn about unsupported deserialization.
     *  Effective deserialzation must be implemented by inheriting classes.
     *  Be careful with template specialization and function member overriding in derived classes.
     */
    virtual bool fromJsonInternal( const nlohmann::json& data );
    /**
     *  \brief Internal json representation of the Node.
     *
     *  Default implementation warn about unsupported deserialization.
     *  Effective deserialzation must be implemented by inheriting classes.
     *  Be careful with template specialization and function member overriding in derived classes.
     */
    virtual void toJsonInternal( nlohmann::json& data ) const;
    /**
     * \brief Adds a port to port collection
     *
     * \param port The in port to add.
     * \param coll Port collection (input or output)
     * \return The index of the inserted port.
     */
    template <typename PortType>
    PortIndex add_port( PortCollection<PortPtr<PortType>>&, PortPtr<PortType> port );
    /// Convenience alias to add_port(inputs(), in)
    PortIndex add_input( PortBaseInPtr in );
    /// Convenience alias to add_port(outputs(), out)
    PortIndex add_output( PortBaseOutPtr out );

    /**
     * \brief Adds a typed input port
     *
     * \tparam T Port data type.
     * \param u Port ctor parameter.
     * \return Typed port shared pointer.
     */
    template <typename T, typename... U>
    PortInPtr<T> add_input( U&&... u ) {
        auto idx = add_input( std::make_shared<PortIn<T>>( this, std::forward<U>( u )... ) );
        return input_port<T>( idx );
    }
    /// \copydoc    template <typename T, typename... U> PortInPtr<T> add_input_port( U&&... u )
    template <typename T, typename... U>
    PortOutPtr<T> add_output( U&&... u ) {
        auto idx = add_output( std::make_shared<PortOut<T>>( this, std::forward<U>( u )... ) );
        return output_port<T>( idx );
    }

    template <typename T>
    PortInPtr<T> input_port( PortIndex index ) {
        return std::static_pointer_cast<PortIn<T>>( m_inputs[index] );
    }

    template <typename T>
    PortOutPtr<T> output_port( PortIndex index ) {
        return std::static_pointer_cast<PortOut<T>>( m_outputs[index] );
    }

    PortBaseInPtr input_port( PortIndex index ) { return m_inputs[index]; }
    PortBaseOutPtr output_port( PortIndex index ) { return m_outputs[index]; }

    template <typename T>
    ParamHandle<T> add_parameter( const std::string& name, const T& value ) {
        return m_parameters.insertVariable<T>( name, value ).first;
    }

    template <typename T>
    bool remove_parameter( const std::string& name ) {
        return m_parameters.deleteVariable( name );
    }

    template <typename T>
    bool remove_parameter( ParamHandle<T>& handle ) {
        return m_parameters.deleteVariable( handle );
    }

    /// Flag that checks if the node is already initialized.
    bool m_initialized { false };
    /// The type name of the node. Initialized once at construction
    std::string m_model_name;
    /// The instance name of the node
    std::string m_instance_name;
    /// Node's name if needed for display
    std::string m_display_name { "" };

    /// The in ports of the node (own by the node)
    PortCollection<PortPtr<PortBaseIn>> m_inputs;
    /// The out ports of the node  (own by the node)
    PortCollection<PortPtr<PortBaseOut>> m_outputs;

    /// The editable parameters of the node
    Ra::Core::VariableSet m_parameters;
    /// To edit input port's default values, filled by input_variables. Maybe not needed, and
    /// input_variables just return a variable set ?
    Ra::Core::VariableSet m_input_variables;

    /// Additional data on the node, added by application or gui or ...
    nlohmann::json m_metadata;
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
    return m_metadata;
}

inline const std::string& Node::model_name() const {
    return m_model_name;
}

inline const std::string& Node::instance_name() const {
    return m_instance_name;
}

inline void Node::set_instance_name( const std::string& newName ) {
    m_instance_name = newName;
}

inline const Node::PortBaseInCollection& Node::inputs() const {
    return m_inputs;
}

inline const Node::PortBaseOutCollection& Node::outputs() const {
    return m_outputs;
}

inline bool Node::operator==( const Node& node ) {
    return ( m_model_name == node.model_name() ) && ( m_instance_name == node.instance_name() );
}

template <typename PortType>
inline Node::PortIndex Node::add_port( PortCollection<PortPtr<PortType>>& ports,
                                       PortPtr<PortType> port ) {
    PortIndex index;
    // look for a free slot
    auto it = std::find_if( ports.begin(), ports.end(), []( const auto& p ) { return !p; } );
    if ( it != ports.end() ) {
        it->swap( port );
        index = std::distance( ports.begin(), it );
    }
    else {
        ports.push_back( std::move( port ) );
        index = ports.size() - 1;
    }
    return index;
}

inline Node::PortIndex Node::add_input( PortBaseInPtr in ) {
    return add_port( m_inputs, std::move( in ) );
}

inline Node::PortIndex Node::add_output( PortBaseOutPtr out ) {
    return add_port( m_outputs, std::move( out ) );
}

inline bool Node::compile() {
    return true;
}

inline Ra::Core::VariableSet& Node::input_variables() {
    m_input_variables.clear();
    for ( const auto& p : m_inputs ) {
        if ( p->has_default_value() ) p->insert( m_input_variables );
    }

    return m_input_variables;
}

inline bool Node::is_output() {
    bool ret = true;
    for ( const auto& p : m_outputs ) {
        ret = ret && ( p->link_count() == 0 );
    }
    return ret;
}

inline bool Node::is_input() {
    bool ret = true;
    //
    for ( const auto& p : m_inputs ) {
        ret = ret && p->has_default_value() && !p->is_linked();
    }
    return ret;
}

template <typename PortType>
auto Node::port_by_name( const PortCollection<PortPtr<PortType>>& ports,
                         const std::string& name ) const -> IndexAndPort<PortRawPtr<PortType>> {
    auto itr = std::find_if(
        ports.begin(), ports.end(), [n = name]( const auto& p ) { return p->name() == n; } );
    PortRawPtr<PortType> port { nullptr };
    PortIndex portIndex;
    if ( itr != ports.cend() ) {
        port      = itr->get();
        portIndex = std::distance( ports.begin(), itr );
    }
    return { portIndex, port };
}

template <typename PortType>
auto Node::port_base( const PortCollection<PortPtr<PortType>>& ports, PortIndex index ) const
    -> PortRawPtr<PortType> {
    if ( 0 <= index && size_t( index ) < ports.size() ) { return ports[index].get(); }
    return nullptr;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
