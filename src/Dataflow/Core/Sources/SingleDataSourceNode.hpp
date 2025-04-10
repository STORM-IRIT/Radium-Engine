#pragma once

#include <Core/Containers/VariableSet.hpp>
#include <Core/Utils/TypesUtils.hpp>
#include <Dataflow/Core/Node.hpp>

#include <iostream>
#include <nlohmann/json.hpp>
#include <utility>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sources {

/**
 * \brief Base class for nodes that will give access to some input data to the graph.
 * This class can be used to feed nodes on a dataflow graph with some data coming
 * from outside the graph or from the source node itself.
 *
 * The data delivered by the node can be explicitly set/get or can be made editable.
 *
 * @tparam T The type of the data to serve.
 */
template <typename T>
class SingleDataSourceNode : public Node
{
  protected:
    SingleDataSourceNode( const std::string& instanceName, const std::string& typeName );

  public:
    // warning, hacky specialization for set editable
    explicit SingleDataSourceNode( const std::string& name ) :
        SingleDataSourceNode( name, SingleDataSourceNode<T>::node_typename() ) {}

    bool execute() override;

    /** \brief Set the data to be delivered by the node.
     *
     * Sets port_in_from default value and port_out_to data points to port_in_from
     * @param data
     */
    void set_data( T data );

    /**
     * \brief Get the delivered data
     * @return The non owning pointer (alias) to the delivered data.
     */
    T* data() const;

  protected:
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

    void toJsonInternal( nlohmann::json& data ) const override {
        return Node::toJsonInternal( data );
    }

  private:
    RA_NODE_PORT_IN( T, from );
    RA_NODE_PORT_OUT( T, to );

  public:
    static const std::string& node_typename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename T>
SingleDataSourceNode<T>::SingleDataSourceNode( const std::string& instanceName,
                                               const std::string& typeName ) :
    Node( instanceName, typeName ) {
    m_port_in_from->setDefaultValue( T {} );
    m_port_out_to->set_data( &m_port_in_from->data() );
}

template <typename T>
bool SingleDataSourceNode<T>::execute() {
    // update ouput in case input has changed (if not default value))
    m_port_out_to->set_data( &m_port_in_from->data() );
    return true;
}

template <typename T>
void SingleDataSourceNode<T>::set_data( T data ) {
    m_port_in_from->setDefaultValue( std::move( data ) );
    m_port_out_to->set_data( &m_port_in_from->data() );
}

template <typename T>
T* SingleDataSourceNode<T>::data() const {
    return &( m_port_in_from->data() );
}

template <typename T>
const std::string& SingleDataSourceNode<T>::node_typename() {
    static std::string demangledTypeName =
        std::string { "Source<" } + Ra::Core::Utils::simplifiedDemangledType<T>() + ">";
    return demangledTypeName;
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
