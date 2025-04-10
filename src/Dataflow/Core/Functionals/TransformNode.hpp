#pragma once
#include <Core/Utils/TypesUtils.hpp>
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Transform an iterable collection
 * \tparam coll_t the collection to transform. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 * This node has two inputs :
 *   - data : port accepting a coll_t data. Linking to this port is mandatory
 *   - op : port accepting an operator with profile std::function<v_t( const v_t& )>.
 *   Linking to this port is not mandatory, the operator might be set once for the node.
 *   If this port is linked, the operator will be taken from the port.
 *
 * This node has one output :
 *   - result : port giving a coll_t such that result = std::transform(data, op)
 */
template <typename coll_t, typename v_t = typename coll_t::value_type>
class TransformNode : public Node
{
  public:
    /**
     * Trasformation operator profile
     */
    using TransformOperator = std::function<v_t( const v_t& )>;

    /**
     * \brief Construct an identity transformer
     * \param instanceName
     */
    explicit TransformNode( const std::string& instanceName );

    /**
     * \brief Construct a transformer with the given operator
     * \param instanceName
     * \param op
     */
    TransformNode( const std::string& instanceName, TransformOperator op );

    void init() override;
    bool execute() override;

    /// Sets the operator on the node
    void setOperator( TransformOperator op );

  protected:
    TransformNode( const std::string& instanceName,
                   const std::string& typeName,
                   TransformOperator op );

    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    RA_NODE_PORT_IN( coll_t, data );
    RA_NODE_PORT_IN( TransformOperator, op );
    RA_NODE_PORT_OUT_WITH_DATA( coll_t, result );

  public:
    static const std::string& node_typename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename coll_t, typename v_t>
TransformNode<coll_t, v_t>::TransformNode( const std::string& instanceName ) :
    TransformNode( instanceName, node_typename(), []( v_t ) { return v_t {}; } ) {}

template <typename coll_t, typename v_t>
TransformNode<coll_t, v_t>::TransformNode( const std::string& instanceName, TransformOperator op ) :
    TransformNode( instanceName, node_typename(), op ) {}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::setOperator( TransformOperator op ) {
    m_port_in_op->set_default_value( op );
}

template <typename coll_t, typename v_t>
void TransformNode<coll_t, v_t>::init() {
    Node::init();
    m_result.clear();
}

template <typename coll_t, typename v_t>
bool TransformNode<coll_t, v_t>::execute() {
    const auto& f      = m_port_in_op->data();
    const auto& inData = m_port_in_data->data();
    m_result.clear();
    // m_elements.reserve( inData.size() ); // --> this is not a requirement of
    // SequenceContainer
    std::transform( inData.begin(), inData.end(), std::back_inserter( m_result ), f );

    return true;
}

template <typename coll_t, typename v_t>
const std::string& TransformNode<coll_t, v_t>::node_typename() {
    static std::string demangledName =
        std::string { "Transform<" } + Ra::Core::Utils::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
TransformNode<coll_t, v_t>::TransformNode( const std::string& instanceName,
                                           const std::string& typeName,
                                           TransformOperator op ) :
    Node( instanceName, typeName ) {
    m_port_in_op->set_default_value( op );
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
