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
    explicit SinkNode( const std::string& name ) : SinkNode( name, SinkNode<T>::getTypename() ) {}

    /**
     * \brief initialize the interface port data pointer
     */
    void init() override;
    bool execute() override;

    /**
     * Get the delivered data
     * @return a copy of the delivered data.
     */
    T getData() const;
    /**
     * Get the delivered data
     * @return a const ref to the delivered data.
     */
    const T& getDataByRef() const;

    Node::PortInPtr<T> getInPort() { return m_portIn; }

  protected:
    /// \todo why are these empty ?
    void toJsonInternal( nlohmann::json& ) const override {}
    bool fromJsonInternal( const nlohmann::json& ) override { return true; }

  private:
    /// @{
    /// \brief Alias for the ports (allow simpler access)
    Node::PortInPtr<T> m_portIn;
    Node::PortOutPtr<T> m_portOut;
    /// @}
  public:
    static const std::string& getTypename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename T>
SinkNode<T>::SinkNode( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ),
    m_portIn { addInputPort<T>( "from" ) },
    m_portOut { addOutputPort<T>( "data" ) } {}

template <typename T>
void SinkNode<T>::init() {
    Node::init();
}

template <typename T>
bool SinkNode<T>::execute() {
    if ( m_portIn->hasData() ) {
        m_portOut->setData( &m_portIn->getData() );
        return true;
    }
    return false;
}

template <typename T>
T SinkNode<T>::getData() const {
    return m_portOut->getData();
}

template <typename T>
const T& SinkNode<T>::getDataByRef() const {
    return m_portOut->getData();
}

template <typename T>
const std::string& SinkNode<T>::getTypename() {
    static std::string demangledName =
        std::string { "Sink<" } + Ra::Core::Utils::simplifiedDemangledType<T>() + ">";
    return demangledName;
}

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
