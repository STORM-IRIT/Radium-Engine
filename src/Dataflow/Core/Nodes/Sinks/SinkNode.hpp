#pragma once
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

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    bool fromJsonInternal( const nlohmann::json& data ) override;

  private:
    /// \todo : allow user to specify where to store the data ? (i.e. make this a shared_ptr ?).
    // If yes, add a method setDataStorage(std::shared_ptr<T> v)
    T m_data;

    /// @{
    /// \brief Alias for the ports (allow simpler access)
    PortIn<T>* m_portIn { new PortIn<T>( "from", this ) };
    /// @}
  public:
    static const std::string& getTypename();
};

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

template <typename T>
SinkNode<T>::SinkNode( const std::string& instanceName, const std::string& typeName ) :
    Node( instanceName, typeName ) {

    m_portIn->mustBeLinked();
    addInput( m_portIn );
}

template <typename T>
void SinkNode<T>::init() {
    // this should be done only once (or when the address of local data changes)
    // What if interfaces were not created before init (e.g. call of init on a node not added to a
    // graph) Todo : assert on the existence of the interface
    auto interfacePort = static_cast<PortOut<T>*>( m_interface[0] );
    interfacePort->setData( &m_data );
    Node::init();
}

template <typename T>
bool SinkNode<T>::execute() {
    m_data = m_portIn->getData();
    return true;
}

template <typename T>
T SinkNode<T>::getData() const {
    return m_data;
}

template <typename T>
const T& SinkNode<T>::getDataByRef() const {
    return m_data;
}

template <typename T>
const std::string& SinkNode<T>::getTypename() {
    static std::string demangledName =
        std::string { "Sink<" } + Ra::Dataflow::Core::simplifiedDemangledType<T>() + ">";
    return demangledName;
}

template <typename T>
void SinkNode<T>::toJsonInternal( nlohmann::json& ) const {}

template <typename T>
bool SinkNode<T>::fromJsonInternal( const nlohmann::json& ) {
    return true;
}

} // namespace Sinks
} // namespace Core
} // namespace Dataflow
} // namespace Ra
