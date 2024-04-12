#pragma once

#include <Dataflow/RaDataflow.hpp>

#include "Core/Utils/Log.hpp"

#include <Dataflow/Core/Port.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortBaseIn; /// Forward PortIn classes used by getLink and reflect
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

    /// Allows to get data stored at this port if it is an output port.
    /// This method copy the data onto the given object
    /// @params t The reference to store the data of this port
    template <typename T>
    T& getData();

    /// Check if this port is an output port, then takes a pointer to the data this port will
    /// point
    /// to.
    /// @param data The pointer to the data.
    template <typename T>
    void setData( T* data );

    // called by PortIn when connect
    void increaseLinkCount() {
        ++m_linkCount;
        CORE_ASSERT( m_linkCount >= 0, "link count error" );
    }

    // called by PortIn when disconnect
    void decreaseLinkCount() {
        --m_linkCount;
        CORE_ASSERT( m_linkCount >= 0, "link count error" );
    }
    int getLinkCount() { return m_linkCount; }

  protected:
    /// Constructor.
    /// @param name The name of the port.
    /// @param type The data's type's hash.
    /// @param node The pointer to the node associated with the port.
    PortBaseOut( Node* node, const std::string& name, std::type_index type );
    /// Count how many times the port is linked
    int m_linkCount { 0 };

}; // class PortBaseOut

/**
 * \brief Output port delivering data of Type T.
 * Output port stores a non-owning pointer to the data that will be made available on a connection.
 *
 * \tparam T The type of the delivered data.
 */
template <typename T>
class PortOut : public PortBaseOut
{
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
    PortOut( Node* node, const std::string& name ) : PortBaseOut( node, name, typeid( T ) ) {}

    PortOut( Node* node, T* data, const std::string& name ) :
        PortBaseOut( node, name, typeid( T ) ), m_data { data } {}
    /// @}

    /// Gets a reference to the data this ports points to.
    T& getData() { return *m_data; }
    /// Takes a pointer to the data this port will point to.
    /// @param data The pointer to the data.
    void setData( T* data ) { m_data = data; }
    /// Returns true if the pointer to the data is not null.
    bool hasData() override { return ( m_data ); }

  private:
    T* m_data { nullptr }; ///< The data the port points to. Use raw ptr, data belongs to node
                           ///< and can be plain stack variable

}; // class PortOut<T>

template <typename T>
T& PortBaseOut::getData() {
    auto thisOut = dynamic_cast<PortOut<T>*>( this );
    if ( thisOut ) { return thisOut->getData(); }
    LOG( Ra::Core::Utils::logERROR )
        << "Unable to get data with type " << simplifiedDemangledType<T>() << " on port "
        << getName() << " which expect " << getTypeName() << ".\n";
    std::abort();
}

template <typename T>
void PortBaseOut::setData( T* data ) {
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

} // namespace Core
} // namespace Dataflow
} // namespace Ra
