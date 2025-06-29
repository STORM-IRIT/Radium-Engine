#pragma once
#include "Core/CoreMacros.hpp"
#include "Dataflow/Core/Port.hpp"
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

/**
 * \brief namespace containing template utilities for management of BinaryOpNode content
 */
namespace internal {

/**
 * \brief Type traits giving access to value_type and const ref type
 * \tparam A
 * \tparam is_container
 */
template <typename A, bool is_container = false>
struct ArgTypeHelperInternal {
    using value_type      = A;
    using const_value_ref = const A&;
};

/**
 * \brief Partial specialization for container type
 * \tparam A
 */
template <typename A>
struct ArgTypeHelperInternal<A, true> {
    using value_type      = typename A::value_type;
    using const_value_ref = const typename A::value_type&;
};

/**
 * \brief CRTP
 * \tparam A
 */
template <typename A>
struct ArgTypeHelper : public ArgTypeHelperInternal<A, Ra::Core::Utils::is_container<A>::value> {};

// Find a way to evaluate the copy/move semantic of t_out
/**
 * \brief Manage the call to y = f(a, b) according to inputs aand ouput types of the node
 * \tparam t_a Type of the source data for argument a of the function
 * \tparam t_b Type of the source data for argument b of the function
 * \tparam t_out Type of the ouput data sent by the node
 * \tparam funcType Profile of the operator f
 * \tparam it_a true if t_a is a container
 * \tparam it_b true if t_b is a container
 * \tparam it_out true if t_out is a container
 */
template <typename t_a,
          typename t_b,
          typename t_out,
          typename funcType,
          bool it_a   = Ra::Core::Utils::is_container<t_a>::value,
          bool it_b   = Ra::Core::Utils::is_container<t_b>::value,
          bool it_out = Ra::Core::Utils::is_container<t_out>::value>
struct ExecutorHelper {
    static t_out executeInternal( t_a&, t_b&, funcType ) {
        static_assert( ( ( it_a || it_b ) ? it_out : !it_out ), "Invalid template parameter " );
    }
};

/**
 * \brief Call of an operator to transform two container into another container.
 */
template <typename t_a, typename t_b, typename t_out, typename funcType>
struct ExecutorHelper<t_a, t_b, t_out, funcType, true, true, true> {
    static t_out executeInternal( t_a& a, t_b& b, funcType f ) {
        t_out res;
        std::transform( a.begin(),
                        a.end(),
                        b.begin(),
                        std::back_inserter( res ),
                        [f]( typename ArgTypeHelper<t_a>::const_value_ref x,
                             typename ArgTypeHelper<t_b>::const_value_ref y ) ->
                        typename ArgTypeHelper<t_out>::value_type { return f( x, y ); } );
        return res;
    }
};

/**
 * \brief Call of an operator to transform a container and a scalar into a container.
 */
template <typename t_a, typename t_b, typename t_out, typename funcType>
struct ExecutorHelper<t_a, t_b, t_out, funcType, true, false, true> {
    static t_out executeInternal( t_a& a, t_b& b, funcType f ) {
        t_out res;
        std::transform( a.begin(),
                        a.end(),
                        std::back_inserter( res ),
                        [&b, f]( typename ArgTypeHelper<t_a>::const_value_ref x ) ->
                        typename ArgTypeHelper<t_out>::value_type { return f( x, b ); } );
        return res;
    }
};

/**
 * \brief Call of an operator to transform a scalar and a container  into a container.
 */
template <typename t_a, typename t_b, typename t_out, typename funcType>
struct ExecutorHelper<t_a, t_b, t_out, funcType, false, true, true> {
    static t_out executeInternal( t_a& a, t_b& b, funcType f ) {
        t_out res;
        std::transform( b.begin(),
                        b.end(),
                        std::back_inserter( res ),
                        [&a, f]( typename ArgTypeHelper<t_b>::const_value_ref x ) ->
                        typename ArgTypeHelper<t_out>::value_type { return f( a, x ); } );
        return res;
    }
};

/**
 * \brief Call of an operator to transform two scalars into a scalar.
 */
template <typename t_a, typename t_b, typename t_out, typename funcType>
struct ExecutorHelper<t_a, t_b, t_out, funcType, false, false, false> {
    static t_out executeInternal( t_a& a, t_b& b, funcType f ) { return f( a, b ); }
};
} // namespace internal

/** \brief Apply a binary operation on its input.
 * \tparam t_a type of the first argument
 * \tparam t_b type of the second argument
 * \tparam t_out type of the result
 *
 * This node apply an operator f on its input such that :
 *   - if t_a, t_b and t_out are collections, r[i] = f(a[i], b[i]) for all elements i in the
 * collections.
 *   - if t_a and t_out are collections, t_b an object, r[i] = f(a[i], b) for all elements i in the
 * collections.
 *   - if t_b and t_out are collections, t_a an object, r[i] = f(a, b[i]) for all elements i in the
 * collections.
 *   - if t_a, t_b and t_out are objects, r = f(a, b).
 *   All other configurations of t_a, t_b and t_out are illegal.
 *
 * This node has three inputs :
 *   - a : port accepting the input data of type t_a. Must be linked.
 *   - b : port accepting the input data of type t_b. Must be linked.
 *   - f : port accepting an operator with profile std::function<Res_type( Arg1_type, Arg2_type )>.
 *   Link to this port is not mandatory, the operator might be set once for the node.
 *   If this port is linked, the operator will be taken from the port.
 *
 * This node has one output :
 *   - out : port giving a t_out such that out = std::transform(a, b, f)
 */
template <typename t_a, typename t_b = t_a, typename t_result = t_a>
class BinaryOpNode : public Node
{
  public:
    /**
     * BinaryOp operator profile
     */
    using Arg1_type      = typename internal::ArgTypeHelper<t_a>::const_value_ref;
    using Arg2_type      = typename internal::ArgTypeHelper<t_b>::const_value_ref;
    using Res_type       = typename internal::ArgTypeHelper<t_result>::value_type;
    using BinaryOperator = std::function<Res_type( Arg1_type, Arg2_type )>;

    /**
     * \brief Construct a BinaryOpNode with the given operator
     * \param instanceName
     * \param op
     */
    BinaryOpNode( const std::string& instanceName, std::optional<BinaryOperator> op = {} ) :
        BinaryOpNode( instanceName, node_typename(), op ) {}

    void init() override {
        m_result = t_result {};
        Node::init();
    }

    bool execute() override {

        const auto& f = m_port_in_op->data();
        m_result = internal::ExecutorHelper<t_a, t_b, t_result, BinaryOperator>::executeInternal(
            m_port_in_a->data(), m_port_in_b->data(), f );
        return true;
    }

    /// \brief Sets the operator to be evaluated by the node.
    void set_operator( BinaryOperator op ) { m_port_in_op->set_default_value( op ); }

    static const std::string& node_typename() {
        static std::string demangledName =
            std::string { "BinaryOp<" } + Ra::Core::Utils::simplifiedDemangledType<t_a>() + " x " +
            Ra::Core::Utils::simplifiedDemangledType<t_b>() + " -> " +
            Ra::Core::Utils::simplifiedDemangledType<t_result>() + ">";
        return demangledName;
    }

  protected:
    BinaryOpNode( const std::string& instanceName,
                  const std::string& typeName,
                  std::optional<BinaryOperator> op ) :
        Node( instanceName, typeName ) {
        if ( op ) m_port_in_op->set_default_value( *op );
    }

    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }

    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    /// \brief the used operator

    RA_NODE_PORT_IN( t_a, a );
    RA_NODE_PORT_IN( t_b, b );
    RA_NODE_PORT_IN( BinaryOperator, op );
    RA_NODE_PORT_OUT_WITH_DATA( t_result, result );
};

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
