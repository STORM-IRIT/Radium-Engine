#pragma once
#include <Dataflow/Core/Node.hpp>

#include <functional>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Functionals {

template <typename Input, typename Output = Input>
class FunctionNode : public Node
{
  public:
    using Function = std::function<Output( const Input& )>;
    static auto IdentityFunction( const Input& x ) -> Output { return Output { x }; }

    FunctionNode( const std::string& instanceName, Function function = IdentityFunction );

    void init() override { Node::init(); }
    bool execute() override;

    void setFunction( Function function ) { m_port_in_op->set_default_value( function ); }

    static const std::string& node_typename();

  protected:
    FunctionNode( const std::string& instanceName, const std::string& typeName, Function function );

    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    RA_NODE_PORT_IN( Input, data );
    RA_NODE_PORT_IN( Function, op );
    RA_NODE_PORT_OUT_WITH_DATA( Output, result );
};

// -----------------------------------------------------------------

template <typename Input, typename Output>
FunctionNode<Input, Output>::FunctionNode( const std::string& instanceName, Function function ) :
    FunctionNode( instanceName, node_typename(), function ) {}

template <typename Input, typename Output>
bool FunctionNode<Input, Output>::execute() {
    const auto& f = m_port_in_op->data();
    const auto& x = m_port_in_data->data();

    m_result = f( x );

    return true;
}

template <typename Input, typename Output>
const std::string& FunctionNode<Input, Output>::node_typename() {
    static std::string demangledName =
        std::string { "Function<" } + Ra::Core::Utils::simplifiedDemangledType<Input>() + ">";
    return demangledName;
}

template <typename Input, typename Output>
FunctionNode<Input, Output>::FunctionNode( const std::string& instanceName,
                                           const std::string& typeName,
                                           Function function ) :
    Node( instanceName, typeName ) {
    m_port_in_op->set_default_value( function );
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
