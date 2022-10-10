#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

namespace internal {
template <typename A, bool is_container = false>
struct ArgTypeHelperInternal {
    using value_type      = A;
    using const_value_ref = const A&;
};

template <typename A>
struct ArgTypeHelperInternal<A, true> {
    using value_type      = typename A::value_type;
    using const_value_ref = const typename A::value_type&;
};

template <typename A>
struct ArgTypeHelper : public ArgTypeHelperInternal<A, Ra::Core::Utils::is_container<A>::value> {};

// Find a way to evaluate the copy/move semantic of t_out
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

template <typename t_a, typename t_b, typename t_out, typename funcType>
struct ExecutorHelper<t_a, t_b, t_out, funcType, false, false, false> {
    static t_out executeInternal( t_a& a, t_b& b, funcType f ) { return f( a, b ); }
};
} // namespace internal
/** \brief Apply a binary operation on its input
 * \tparam v_t type of the element to apply the operator on
 */
template <typename t_a, typename t_b = t_a, typename t_out = t_a>
class BinaryOpNode : public Node
{
  public:
    /**
     * BinaryOp operator profile
     */
    using Arg1_type      = typename internal::ArgTypeHelper<t_a>::const_value_ref;
    using Arg2_type      = typename internal::ArgTypeHelper<t_b>::const_value_ref;
    using Res_type       = typename internal::ArgTypeHelper<t_out>::value_type;
    using BinaryOperator = std::function<Res_type( Arg1_type, Arg2_type )>;

    /**
     * \brief Construct an additioner
     * \param instanceName
     */
    explicit BinaryOpNode( const std::string& instanceName ) :
        BinaryOpNode( instanceName, getTypename(), []( Arg1_type, Arg2_type ) -> Res_type {
            return Res_type {};
        } ) {}

    /**
     * \brief Construct a BinaryOpNode with the given operator
     * \param instanceName
     * \param filterFunction
     */
    BinaryOpNode( const std::string& instanceName, BinaryOperator op ) :
        BinaryOpNode( instanceName, getTypename(), op ) {}

    void init() override {
        Node::init();
        m_result = t_out {};
    }
    void execute() override {
        auto f   = m_operator;
        auto p_f = static_cast<PortIn<BinaryOperator>*>( m_inputs[2].get() );
        if ( p_f->isLinked() ) { f = p_f->getData(); }
        auto p_a = static_cast<PortIn<t_a>*>( m_inputs[0].get() );
        auto p_b = static_cast<PortIn<t_b>*>( m_inputs[1].get() );
        if ( p_a->isLinked() && p_b->isLinked() ) {
            m_result = internal::ExecutorHelper<t_a, t_b, t_out, BinaryOperator>::executeInternal(
                p_a->getData(), p_b->getData(), f );
        }
    }

    /// Sets the operator on the node
    void setOperator( BinaryOperator op ) { m_operator = op; }

  protected:
    BinaryOpNode( const std::string& instanceName,
                  const std::string& typeName,
                  BinaryOperator op ) :
        Node( instanceName, typeName ), m_operator( op ) {
        auto in_a = new PortIn<t_a>( "a", this );
        addInput( in_a );
        in_a->mustBeLinked();
        auto in_b = new PortIn<t_b>( "b", this );
        addInput( in_b );
        in_b->mustBeLinked();
        auto f = new PortIn<BinaryOperator>( "f", this );
        addInput( f );
        auto out = new PortOut<t_out>( "r", this );
        addOutput( out, &m_result );
    }

    void toJsonInternal( nlohmann::json& data ) const override {
        data["comment"] =
            std::string { "Binary operator could not be serialized for " } + getTypeName();
        LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
            << "Unable to save data when serializing a " << getTypeName() << ".";
    }

    void fromJsonInternal( const nlohmann::json& ) override {
        LOG( Ra::Core::Utils::logWARNING ) // TODO make this logDEBUG
            << "Unable to read data when un-serializing a " << getTypeName() << ".";
    }

  private:
    BinaryOperator m_operator = []( Arg1_type, Arg2_type ) -> Res_type { return Res_type {}; };
    t_out m_result;

  public:
    static const std::string& getTypename() {
        static std::string demangledName =
            std::string { "BinaryOp<" } + Ra::Dataflow::Core::simplifiedDemangledType<t_a>() +
            " x " + Ra::Dataflow::Core::simplifiedDemangledType<t_b>() + " -> " +
            Ra::Dataflow::Core::simplifiedDemangledType<t_out>() + ">";
        return demangledName;
    }
};

// implementation of the methods
// see issue .inl coding style #1011

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
