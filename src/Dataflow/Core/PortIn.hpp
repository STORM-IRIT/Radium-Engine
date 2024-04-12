#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <optional>

#include "Core/Utils/Log.hpp"
#include <Core/Utils/Observable.hpp>

#include <Dataflow/Core/Port.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortBaseOut; /// Forward PortOut classes used by getLink and reflect
template <typename T>
class PortOut;

class RA_DATAFLOW_API PortBaseIn : public PortBase
{
  public:
    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortBaseIn()                               = delete;
    PortBaseIn( const PortBaseIn& )            = delete;
    PortBaseIn& operator=( const PortBaseIn& ) = delete;
    /// @}

    virtual bool accept( PortBaseOut* portOut ) const;
    virtual bool connect( PortBaseOut* portOut ) = 0;
    virtual bool disconnect()                    = 0;

    virtual bool isLinkMandatory() const;
    virtual bool isLinked() const        = 0;
    virtual bool hasDefaultValue() const = 0;

    virtual PortBaseOut* getLink() = 0;

    template <typename T>
    PortOut<T>* getLinkAs();

    template <typename T>
    void setDefaultValue( const T& value );

    template <typename T>
    T& getData();

  protected:
    /// Constructor.
    /// @param name The name of the port.
    /// @param type The data's type's hash.
    /// @param node The pointer to the node associated with the port.
    PortBaseIn( Node* node, const std::string& name, std::type_index type );

}; // class PortBaseIn

/**
 * \brief Input port accepting data of type T.
 * An input port does not staore the data but is an accessor to the data stored on the connected
 * output port. An Input port is observable and notify its observers at each  connect/disconnect
 * event.
 *
 * \tparam T The accepted data type
 */
template <typename T>
class PortIn : public PortBaseIn,
               public Ra::Core::Utils::Observable<const std::string&, const PortIn<T>&, bool>
{
  public:
    using DataType = T;

    /// \name Constructors
    /// @{
    /// \brief delete default constructors.
    PortIn()                           = delete;
    PortIn( const PortIn& )            = delete;
    PortIn& operator=( const PortIn& ) = delete;

    /// Constructor.
    /// @param node The pointer to the node associated with the port.
    /// @param name The name of the port.
    PortIn( Node* node, const std::string& name ) : PortBaseIn( node, name, typeid( T ) ) {}
    /// @}

    /// Returns true if the port is linked to an output port that has data.
    bool hasData() override;

    ///\brief Gets the data pointed by the connected out port.
    ///
    /// It checks if the data is available. If it is not, it returns the default value.
    ///
    ///\return T& The reference to the data.
    T& getData();

    /// \name Manage the connection with a PortOut.
    /// @{
    bool accept( PortBaseOut* portOut ) const override;
    bool accept( PortOut<T>* portOut ) const;

    bool connect( PortBaseOut* portOut ) override;
    bool connect( PortOut<T>* portOut );

    bool disconnect() override;

    PortBaseOut* getLink() override { return m_from; }
    /// @}

    bool isLinkMandatory() const override { return !m_defaultValue.has_value(); }
    void setDefaultValue( const T& value ) { m_defaultValue = value; }
    bool hasDefaultValue() const override { return m_defaultValue.has_value(); }
    bool isLinked() const override { return m_from != nullptr; }

  private:
    PortOut<T>* m_from = nullptr;       ///< A pointer to the out port this port is connected to.
    std::optional<T> m_defaultValue {}; ///< The value used when not connected.

}; // class PortIn<T>

template <typename T>
PortOut<T>* PortBaseIn::getLinkAs() {
    return static_cast<PortIn<T>*>( this )->getLink();
}

template <typename T>
void PortBaseIn::setDefaultValue( const T& value ) {
    static_cast<PortIn<T>*>( this )->setDefaultValue( value );
}

template <typename T>
T& PortBaseIn::getData() {
    return static_cast<PortIn<T>*>( this )->getData();
}

template <typename T>
T& PortIn<T>::getData() {
    if ( isLinked() ) return m_from->getData();
    if ( m_defaultValue ) return *m_defaultValue;
    CORE_ASSERT( false, "should not get here" );
    using namespace Ra::Core::Utils;
    LOG( logERROR ) << "graph is not valid";
    return *m_defaultValue;
}

template <typename T>
inline bool PortIn<T>::hasData() {
    if ( isLinked() ) { return m_from->hasData(); }
    return m_defaultValue.has_value();
}

template <typename T>
bool PortIn<T>::accept( PortBaseOut* portOut ) const {
    return !m_from && ( PortBaseIn::accept( portOut ) );
}

template <typename T>
bool PortIn<T>::accept( PortOut<T>* ) const {
    return !m_from;
}

template <typename T>
bool PortIn<T>::connect( PortBaseOut* portOut ) {
    if ( accept( portOut ) ) {
        m_from = static_cast<PortOut<T>*>( portOut );
        m_from->increaseLinkCount();
        // notify after connect
        this->notify( getName(), *this, true );
    }
    return ( m_from );
}

template <typename T>
bool PortIn<T>::connect( PortOut<T>* portOut ) {
    if ( accept( portOut ) ) {
        m_from = portOut;
        m_from->increaseLinkCount();
        // notify after connect
        this->notify( getName(), *this, true );
    }
    return ( m_from );
}

template <typename T>
bool PortIn<T>::disconnect() {
    if ( m_from ) {
        // notify before disconnect
        m_from->decreaseLinkCount();
        this->notify( getName(), *this, false );
        m_from = nullptr;
        return true;
    }
    return false;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
