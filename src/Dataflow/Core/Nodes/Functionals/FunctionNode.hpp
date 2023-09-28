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

    void setFunction( Function function ) { m_portFunction->setDefaultValue( function ); }
    Node::PortInPtr<Input> getInPort() { return m_portIn; }
    Node::PortInPtr<Function> getFunctionPort() { return m_portFunction; }
    Node::PortOutPtr<Output> getOutPort() { return m_portOut; }

    static const std::string& getTypename();

  protected:
    FunctionNode( const std::string& instanceName, const std::string& typeName, Function function );

    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    Output m_result;

    Node::PortInPtr<Input> m_portIn;
    Node::PortInPtr<Function> m_portFunction;
    Node::PortOutPtr<Output> m_portOut;
};

// -----------------------------------------------------------------

template <typename Input, typename Output>
FunctionNode<Input, Output>::FunctionNode( const std::string& instanceName, Function function ) :
    FunctionNode( instanceName, getTypename(), function ) {}

template <typename Input, typename Output>
bool FunctionNode<Input, Output>::execute() {
    const auto& f = m_portFunction->getData();
    const auto& x = m_portIn->getData();

    m_result = f( x );

    return true;
}

template <typename Input, typename Output>
const std::string& FunctionNode<Input, Output>::getTypename() {
    static std::string demangledName =
        std::string { "Function<" } + Ra::Dataflow::Core::simplifiedDemangledType<Input>() + ">";
    return demangledName;
}

template <typename Input, typename Output>
FunctionNode<Input, Output>::FunctionNode( const std::string& instanceName,
                                           const std::string& typeName,
                                           Function function ) :
    Node( instanceName, typeName ),
    m_portIn { addInputPort<Input>( "in" ) },
    m_portFunction { addInputPort<Function>( "f" ) },
    m_portOut { addOutputPort<Input>( &m_result, "out" ) } {
    m_portFunction->setDefaultValue( function );
}

} // namespace Functionals
} // namespace Core
} // namespace Dataflow
} // namespace Ra
