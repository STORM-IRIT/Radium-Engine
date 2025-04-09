#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/** \brief Filter on iterable collection.
 * \tparam coll_t the collection to filter. Must respect the SequenceContainer requirements
 * \tparam v_t (optional), type of the element in the collection. Default to coll_t::value_type
 * \see https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 * This node apply an operator f on its input such that to keep only elements validated by a
 * predicate :
 *
 * This node has two inputs :
 *   - data : port accepting the input data of type coll_t. Must be linked.
 *   - predicate : port accepting an operator with profile std::function<bool( const v_t& )>.
 *   Link to this port is not mandatory, the operator might be set once for the node.
 *   If this port is linked, the operator will be taken from the port.
 *
 * This node has one output :
 *   - result : port giving a coll_t such that out = std::copy_if(a, f)
 */
template <typename coll_t, typename v_t = typename coll_t::value_type>
class FilterNode : public Node
{
  public:
    /**
     * unaryPredicate Type
     */
    using UnaryPredicate = std::function<bool( const v_t& )>;

    /**
     * \brief Construct a filter accepting all its input ( true() lambda )
     * \param instanceName
     */
    explicit FilterNode( const std::string& instanceName );

    /**
     * \brief Construct a filter with the given predicate
     * \param instanceName
     * \param predicate
     */
    FilterNode( const std::string& instanceName, UnaryPredicate predicate );

    void init() override;
    bool execute() override;

    /// Sets the filtering predicate on the node
    void set_predicate( UnaryPredicate predicate );

  protected:
    FilterNode( const std::string& instanceName,
                const std::string& typeName,
                UnaryPredicate predicate );

    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    RA_NODE_PORT_IN( coll_t, data );
    RA_NODE_PORT_IN( UnaryPredicate, predicate );
    RA_NODE_PORT_OUT_WITH_DATA( coll_t, result );

  public:
    static const std::string& node_typename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName ) :
    FilterNode( instanceName, node_typename(), []( v_t ) { return true; } ) {}

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName, UnaryPredicate predicate ) :
    FilterNode( instanceName, node_typename(), predicate ) {}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::set_predicate( UnaryPredicate predicate ) {
    m_port_in_predicate->setDefaultValue( predicate );
}

template <typename coll_t, typename v_t>
void FilterNode<coll_t, v_t>::init() {
    Node::init();
    m_result.clear();
}

template <typename coll_t, typename v_t>
bool FilterNode<coll_t, v_t>::execute() {
    const auto& f      = m_port_in_predicate->getData();
    const auto& inData = m_port_in_data->getData();
    m_result.clear();
    // since we do not know how many inData respect the predicate, do not reserve m_result
    std::copy_if( inData.begin(), inData.end(), std::back_inserter( m_result ), f );

    return true;
}

template <typename coll_t, typename v_t>
const std::string& FilterNode<coll_t, v_t>::node_typename() {
    static std::string demangledName =
        std::string { "Filter<" } + Ra::Core::Utils::simplifiedDemangledType<coll_t>() + ">";
    return demangledName;
}

template <typename coll_t, typename v_t>
FilterNode<coll_t, v_t>::FilterNode( const std::string& instanceName,
                                     const std::string& typeName,
                                     UnaryPredicate predicate ) :
    Node( instanceName, typeName ) {
    m_port_in_predicate->setDefaultValue( predicate );
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
