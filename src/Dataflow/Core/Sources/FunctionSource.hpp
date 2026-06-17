#pragma once
#include <Dataflow/Core/NodeFactory.hpp>
#include <Dataflow/RaDataflow.hpp>
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
    RA_NODE_TYPENAME( std::string { "Source<" } +
                      Ra::Core::Utils::simplifiedDemangledType<function_type>() + ">" );

    bool execute() override;

    /** \brief Set the function to be delivered by the node.
     * @param data
     */
    void set_data( function_type data );

    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    function_type* data() const;

    RA_NODE_PORT_IN( function_type, from );
    RA_NODE_PORT_OUT( function_type, to );

  protected:
    FunctionSourceNode( const std::string& instanceName, const std::string& typeName );

    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }
    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <class R, class... Args>
FunctionSourceNode<R, Args...>::FunctionSourceNode( const std::string& instanceName,
                                                    const std::string& typeName ) :
    Node( instanceName, typeName ) {
    m_port_in_from->set_default_value( []( Args... ) { return R {}; } );
    m_port_out_to->set_data( &m_port_in_from->data() );
}

template <class R, class... Args>
bool FunctionSourceNode<R, Args...>::execute() {
    return true;
}

template <class R, class... Args>
void FunctionSourceNode<R, Args...>::set_data( function_type data ) {
    m_port_in_from->set_default_value( std::move( data ) );
    m_port_out_to->set_data( &m_port_in_from->data() );
}

template <class R, class... Args>
typename FunctionSourceNode<R, Args...>::function_type*
FunctionSourceNode<R, Args...>::data() const {
    return m_port_in_from->data();
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
