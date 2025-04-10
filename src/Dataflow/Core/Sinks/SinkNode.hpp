#pragma once
#include <Core/Utils/TypesUtils.hpp>
#include <Dataflow/Core/Node.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {
namespace Sinks {

/**
 * \brief Base class for nodes that will store the result of a computation graph.
 * @tparam T The type of the data to serve.
 */
template <typename T>
class SinkNode : public Node
{
  protected:
    SinkNode( const std::string& instanceName, const std::string& typeName );

  public:
    explicit SinkNode( const std::string& name ) : SinkNode( name, SinkNode<T>::node_typename() ) {}

    /**
     * \brief initialize the interface port data pointer
     */
    void init() override;
    bool execute() override;

    /**
     * Get the delivered data
     * \return a copy of the delivered data.
     */
    T data() const;
    /**
     * Get the delivered data
     * \return a const ref to the delivered data.
     */
    const T& data_reference() const;

  protected:
    void toJsonInternal( nlohmann::json& data ) const override { Node::toJsonInternal( data ); }
    bool fromJsonInternal( const nlohmann::json& data ) override {
        return Node::fromJsonInternal( data );
    }

  private:
    /// @{
    /// \brief Alias for the ports (allow simpler access)
    RA_NODE_PORT_IN( T, from );
    RA_NODE_PORT_OUT( T, data );
    /// @}
  public:
    static const std::string& node_typename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename T>
SinkNode<T>::SinkNode( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {}

template <typename T>
void SinkNode<T>::init() {
    Node::init();
}

template <typename T>
bool SinkNode<T>::execute() {
    if ( m_port_in_from->has_data() ) {
        m_port_out_data->set_data( &m_port_in_from->data() );
        return true;
    }
    return false;
}

template <typename T>
T SinkNode<T>::data() const {
    return m_port_out_data->data();
}

template <typename T>
const T& SinkNode<T>::data_reference() const {
    return m_port_out_data->data();
}

template <typename T>
const std::string& SinkNode<T>::node_typename() {
    static std::string demangledName =
        std::string { "Sink<" } + Ra::Core::Utils::simplifiedDemangledType<T>() + ">";
    return demangledName;
}

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
