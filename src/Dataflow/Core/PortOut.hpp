#pragma once

#include <Dataflow/Core/Port.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortBaseIn;
template <typename T>
class PortIn;

class RA_DATAFLOW_API PortBaseOut : public PortBase
{
  public:
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortBaseOut()                                = delete;
    PortBaseOut( const PortBaseOut& )            = delete;
    PortBaseOut& operator=( const PortBaseOut& ) = delete;
    /// @}

    /// Returns a reflected (In <-> Out) port of the same type
    /// TODO : remove interface ? so remove reflect ?
    virtual PortBaseIn* reflect( Node* node, std::string name ) const = 0;

    template <typename T>
    T& getData();

    template <typename T>
    void setData( T* data );

  protected:
    /// Constructor.
    /// @param name The name of the port.
    /// @param type The data's type's hash.
    /// @param node The pointer to the node associated with the port.
    PortBaseOut( Node* node, const std::string& name, std::type_index type );
}; // class PortBaseOut

/**
 * \brief Output port delivering data of Type T.
 * Output port stores a non-owning pointer to the data that will be made available on a connection.
 * \tparam T The type of the delivered data.
 */
template <typename T>
class PortOut : public PortBaseOut
{
  private:
    /// The data the port points to.

    /// Use raw ptr, data belongs to node and can be plain stack variable
    T* m_data { nullptr };

  public:
    using DataType = T;
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortOut()                            = delete;
    PortOut( const PortOut& )            = delete;
    PortOut& operator=( const PortOut& ) = delete;
    /// Constructor.
    /// @param name The name of the port.
    /// @param node The pointer to the node associated with the port.
    /// \todo remove this one
    PortOut( const std::string& name, Node* node );
    PortOut( Node* node, const std::string& name );
    PortOut( Node* node, T* data, const std::string& name );
    /// @}

    /// Gets a reference to the data this ports points to.
    T& getData();
    /// Takes a pointer to the data this port will point to.
    /// @param data The pointer to the data.
    void setData( T* data );
    /// Returns true if the pointer to the data is not null.
    bool hasData() override;
    /// Returns nullptr because this port is an out port.
    // PortBase* getLink() override;
    /// Returns false because out ports can not accept connection.
    /// @param o The other port to test the connection.
    // bool accept( PortBase* ) override;
    /// Calls the connect(PortBase* o) function of the o node because out ports can not connect.
    /// @param o The other port to connect.
    // bool connect( PortBase* o ) override;
    /// Returns false because out ports can not disconnect.
    // bool disconnect() override;
    /// Returns a portIn of the same type
    PortBaseIn* reflect( Node* node, std::string name ) const override;
}; // class PortOut<T>

} // namespace Core
} // namespace Dataflow
} // namespace Ra

namespace Ra {
namespace Dataflow {
namespace Core {
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PortBaseOut ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T>
T& PortBaseOut::getData() {
    return static_cast<PortOut<T>*>( this )->getData();
}

template <typename T>
void PortBaseOut::setData( T* data ) {
    static_cast<PortOut<T>*>( this )->setData( data );
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PortOut ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T>
PortOut<T>::PortOut( const std::string& name, Node* node ) :
    PortBaseOut( node, name, typeid( T ) ) {}

template <typename T>
PortOut<T>::PortOut( Node* node, const std::string& name ) :
    PortBaseOut( node, name, typeid( T ) ) {}

template <typename T>
PortOut<T>::PortOut( Node* node, T* data, const std::string& name ) :
    PortBaseOut( node, name, typeid( T ) ), m_data { data } {}

template <typename T>
T& PortOut<T>::getData() {
    return *m_data;
}

template <typename T>
void PortOut<T>::setData( T* data ) {
    m_data = data;
}

template <typename T>
bool PortOut<T>::hasData() {
    return m_data != nullptr;
}

template <typename T>
void PortBase::getData( T& t ) {
    if ( !is_input() ) {
        auto thisOut = dynamic_cast<PortOut<T>*>( this );
        if ( thisOut ) {
            t = thisOut->getData();
            return;
        }
        LOG( Ra::Core::Utils::logERROR )
            << "Unable to get data with type " << simplifiedDemangledType<T>() << " on port "
            << getName() << " which expect " << getTypeName() << ".\n";
        std::abort();
    }
    LOG( Ra::Core::Utils::logERROR )
        << "Could not call PortBase::getData( T& t ) on the input port " << getName() << ".\n";
    std::abort();
}

template <typename T>
T& PortBase::getData() {
    if ( !is_input() ) {
        auto thisOut = dynamic_cast<PortOut<T>*>( this );
        if ( thisOut ) { return thisOut->getData(); }
        LOG( Ra::Core::Utils::logERROR )
            << "Unable to get data with type " << simplifiedDemangledType<T>() << " on port "
            << getName() << " which expect " << getTypeName() << ".\n";
        std::abort();
    }
    LOG( Ra::Core::Utils::logERROR )
        << "Could not call T& PortBase::getData<T>() on the input port " << getName() << ".\n";
    std::abort();
}

template <typename T>
void PortBase::setData( T* data ) {
    if ( !is_input() ) {
        auto thisOut = dynamic_cast<PortOut<T>*>( this );
        if ( thisOut ) {
            thisOut->setData( data );
            return;
        }
        LOG( Ra::Core::Utils::logERROR )
            << "Unable to set data with type " << simplifiedDemangledType( *data ) << " on port "
            << getName() << " which expect " << getTypeName() << ".\n";
        std::abort();
    }
    LOG( Ra::Core::Utils::logERROR )
        << "Could not call  PortBase::setData(T* data) on the input port " << getName() << ".\n";
    std::abort();
}

template <typename T>
PortBaseIn* PortOut<T>::reflect( Node* node, std::string name ) const {
    return new PortIn<DataType>( name, node );
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
