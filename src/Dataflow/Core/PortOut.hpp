#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <Core/Utils/Log.hpp>
#include <Core/Utils/TypesUtils.hpp>

#include <Dataflow/Core/Port.hpp>
#include <stdexcept>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortBaseIn; /// Forward PortIn classes used by getLink and reflect
template <typename T>
class PortIn;

class RA_DATAFLOW_CORE_API PortBaseOut : public PortBase
{
  public:
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortBaseOut()                                = delete;
    PortBaseOut( const PortBaseOut& )            = delete;
    PortBaseOut& operator=( const PortBaseOut& ) = delete;
    /// @}

    /// Get data stored at this outpute port
    /// Check if this port type it the same as T
    template <typename T>
    T& data();

    /// Set port data pointer.
    /// Check if this port type it the same as T
    /// @param data The pointer to the data.
    template <typename T>
    void set_data( T* data );

    // called by PortIn when connect
    virtual void increase_link_count() {
        ++m_linkCount;
        CORE_ASSERT( m_linkCount >= 0, "link count error" );
    }

    // called by PortIn when disconnect
    virtual void decrease_link_count() {
        --m_linkCount;
        CORE_ASSERT( m_linkCount >= 0, "link count error" );
    }
    virtual int link_count() { return m_linkCount; }

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
    PortOut( Node* node, const std::string& name ) : PortBaseOut( node, name, typeid( T ) ) {
        add_port_type<T>();
    }

    PortOut( Node* node, T* data, const std::string& name ) :
        PortBaseOut( node, name, typeid( T ) ), m_data { data } {
        add_port_type<T>();
    }
    /// @}

    /// Gets a reference to the data this ports points to.
    T& data() { return *m_data; }
    /// Takes a pointer to the data this port will point to.
    /// @param data The pointer to the data.
    void set_data( T* data ) { m_data = data; }
    /// Returns true if the pointer to the data is not null.
    bool has_data() override { return ( m_data ); }

  private:
    /** Port's data pointer.
     *
     * Use raw ptr since data belongs to the node and can be plain stack variable
     */
    T* m_data { nullptr };

}; // class PortOut<T>

template <typename Type>
using PortOutPtr = PortPtr<PortOut<Type>>;
template <typename Type>
using PortOutRawPtr = typename PortOutPtr<Type>::element_type*;

using PortBaseOutRawPtr = PortRawPtr<PortBaseOut>;
using PortBaseOutPtr    = PortPtr<PortBaseOut>;

template <typename T>
T& PortBaseOut::data() {
    auto thisOut = dynamic_cast<PortOut<T>*>( this );
    if ( thisOut && thisOut->has_data() ) { return thisOut->data(); }

    using namespace Ra::Core::Utils;
    LOG( Ra::Core::Utils::logERROR )
        << "Unable to get data with type " << simplifiedDemangledType<T>() << " on port " << name()
        << " which expect " << port_typename() << ".\n";
    throw std::runtime_error( "data reference do not exists" );
}

template <typename T>
void PortBaseOut::set_data( T* data ) {
    auto thisOut = dynamic_cast<PortOut<T>*>( this );
    if ( thisOut ) {
        thisOut->set_data( data );
        return;
    }

    using namespace Ra::Core::Utils;
    LOG( Ra::Core::Utils::logERROR )
        << "Unable to set data with type " << simplifiedDemangledType( *data ) << " on port "
        << name() << " which expect " << port_typename() << ".\n";
    throw std::runtime_error( "data reference do not exists" );
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
