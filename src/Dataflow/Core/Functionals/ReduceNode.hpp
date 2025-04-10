#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Reduce an iterable collection using a given operator
 * \tparam coll_t the collection to reduce. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 * This node has three inputs :
 *   - in : port accepting a coll_t data. Linking to this port is mandatory
 *   - f : port accepting an operator with profile std::function<v_t( const v_t& )>.
 *   Linking to this port is not mandatory, the operator might be set once for the node or is the
 * default value for v_t. If this port is linked, the operator will be taken from the port.
 *   - init : port accepting a v_t to serve as initial value.
 *   Linking to this port is not mandatory.
 *   If this port is linked, the initial value will be taken from the port.
 *
 * This node has one output :
 *   - out : port giving a v_t such that out = std::reduce(in, f, init) (std::accumulate if less
 * than C++17)
 */
// TODO, allow to specify the type of the reduced information. This will allow, e.g, given a
//  collection of X, to compute a tuple containing mean and standard deviation of the collection
//  as reduction (using online Welford algo).
template <typename coll_t, typename v_t = typename coll_t::value_type>
class ReduceNode : public Node
{
  public:
    /**
     * Trasformation operator profile
     */
    using ReduceOperator = std::function<v_t( const v_t&, const v_t& )>;

    /**
     * \brief Construct an identity transformer
     * \param instanceName
     */
    explicit ReduceNode( const std::string& instanceName );

    /**
     * \brief Construct a transformer with the given operator
     * \param instanceName
     * \param op
     * \param initialValue
     */
    ReduceNode( const std::string& instanceName, ReduceOperator op, v_t initialValue = v_t {} );

    void init() override;
    bool execute() override;

    /// Sets the operator on the node
    void setOperator( ReduceOperator op, v_t initialValue = v_t {} );

  protected:
    ReduceNode( const std::string& instanceName,
                const std::string& typeName,
                ReduceOperator op,
                v_t initialValue );

    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    RA_NODE_PORT_IN( v_t, init );
    RA_NODE_PORT_IN( coll_t, data );
    RA_NODE_PORT_IN( ReduceOperator, op );
    RA_NODE_PORT_OUT_WITH_DATA( v_t, result );

  public:
    static const std::string& node_typename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName ) :
    ReduceNode(
        instanceName,
        node_typename(),
        []( const v_t& a, const v_t& ) -> v_t { return a; },
        v_t {} ) {}

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName,
                                     ReduceOperator op,
                                     v_t initialValue ) :
    ReduceNode( instanceName, node_typename(), op, initialValue ) {}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::setOperator( ReduceOperator op, v_t initialValue ) {
    m_port_in_op->set_default_value( op );
    m_port_in_init->set_default_value( initialValue );
}

template <typename coll_t, typename v_t>
void ReduceNode<coll_t, v_t>::init() {
    Node::init();
    m_result = m_port_in_init->data();
}

template <typename coll_t, typename v_t>
bool ReduceNode<coll_t, v_t>::execute() {
    const auto& f  = m_port_in_op->data();
    const auto& in = m_port_in_data->data();

    m_result = std::accumulate( in.begin(), in.end(), m_port_in_init->data(), f );

    return true;
}

template <typename coll_t, typename v_t>
const std::string& ReduceNode<coll_t, v_t>::node_typename() {
    static std::string demangledName =
        std::string { "Reduce<" } + Ra::Core::Utils::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
ReduceNode<coll_t, v_t>::ReduceNode( const std::string& instanceName,
                                     const std::string& typeName,
                                     ReduceOperator op,
                                     v_t initialValue ) :
    Node( instanceName, typeName ) {
    m_port_in_op->set_default_value( op );
    m_port_in_init->set_default_value( initialValue );
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
