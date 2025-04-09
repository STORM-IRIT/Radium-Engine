#pragma once
#include "Dataflow/Core/NodeFactory.hpp"
#pragma once
#include <Dataflow/Core/Node.hpp>

#include <iostream>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

/**
 * \brief Node that deliver a std::function<R( Args... )}}>
 * \tparam R return type of the function
 * \tparam Type of the function arguments
 */
template <class R, class... Args>
class FunctionSourceNode : public Node
{

  public:
    using function_type = std::function<R( Args... )>;

    explicit FunctionSourceNode( const std::string& name ) :
        FunctionSourceNode( name, FunctionSourceNode<R, Args...>::node_typename() ) {}

    bool execute() override;

    /** \brief Set the function to be delivered by the node.
     * @param data
     */
    void setData( function_type data );

    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    function_type* getData() const;

  protected:
    FunctionSourceNode( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }
    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }

    RA_NODE_PORT_IN( function_type, from );
    RA_NODE_PORT_OUT( function_type, to );

  public:
    static const std::string& node_typename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <class R, class... Args>
FunctionSourceNode<R, Args...>::FunctionSourceNode( const std::string& instanceName,
                                                    const std::string& typeName ) :
    Node( instanceName, typeName ) {
    m_port_in_from->setDefaultValue( []( Args... ) { return R {}; } );
    m_port_out_to->setData( &m_port_in_from->getData() );
}

template <class R, class... Args>
bool FunctionSourceNode<R, Args...>::execute() {
    return true;
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::setData( function_type data ) {
    m_port_in_from->setDefaultValue( std::move( data ) );
    m_port_out_to->setData( &m_port_in_from->getData() );
}

template <class R, class... Args>
typename FunctionSourceNode<R, Args...>::function_type*
FunctionSourceNode<R, Args...>::getData() const {
    return m_port_in_from->getData();
}

template <class R, class... Args>
const std::string& FunctionSourceNode<R, Args...>::node_typename() {
    static std::string demangledTypeName =
        std::string { "Source<" } + Ra::Core::Utils::simplifiedDemangledType<function_type>() + ">";
    return demangledTypeName;
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
