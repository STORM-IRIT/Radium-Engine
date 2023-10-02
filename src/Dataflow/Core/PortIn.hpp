#pragma once

#include <Dataflow/Core/Port.hpp>

#include <Core/Utils/Observable.hpp>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortBaseOut;
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

    /// Returns a reflected (In <-> Out) port of the same type
    /// TODO : remove interface ? so remove reflect ?
    virtual PortBaseOut* reflect( Node* node, std::string name ) const = 0;

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
 * event. \tparam T The accepted data type
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
    /// @param name The name of the port.
    /// @param node The pointer to the node associated with the port.
    ///\todo remove this one
    PortIn( const std::string& name, Node* node );
    PortIn( Node* node, const std::string& name );
    /// @}

    /// Gets the out port this port is connected to.
    PortBaseOut* getLink() override;
    /// Returns true if the port is linked to an output port that has data.
    bool hasData() override;
    /// Gets a reference to the data pointed by the connected out port.
    /// \note no verification is made about the availability of the data.
    T& getData();
    /// Checks if there is not out port already connected and if the data types are the same.
    /// @param o The other port to test the connection
    // bool accept( PortBase* other ) override;
    bool accept( PortBaseOut* portOut ) const override;
    bool accept( PortOut<T>* portOut ) const;
    /// Connects this in port and the other out port if there is no out port already connected and
    /// if the data types are the same.
    /// @param o The other port to connect.
    // bool connect( PortBase* other ) override;
    bool connect( PortBaseOut* portOut ) override;
    bool connect( PortOut<T>* portOut );
    /// Disconnects this port if it is connected.
    bool disconnect() override;
    /// Returns a portOut of the same type
    PortBaseOut* reflect( Node* node, std::string name ) const override;
    /// Returns true if the port is an input port
    bool is_input() override;

    bool isLinkMandatory() const override { return !m_defaultValue.has_value(); }
    void setDefaultValue( const T& value ) { m_defaultValue = value; }
    bool hasDefaultValue() const override { return m_defaultValue.has_value(); }
    bool isLinked() const override { return m_from != nullptr; }

  private:
    /// A pointer to the out port this port is connected to.
    PortOut<T>* m_from = nullptr;
    std::optional<T> m_defaultValue {};
}; // class PortIn<T>

} // namespace Core
} // namespace Dataflow
} // namespace Ra

namespace Ra {
namespace Dataflow {
namespace Core {

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PortBaseIn ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T>
PortOut<T>* PortBaseIn::getLinkAs() {
    return static_cast<PortIn<T>*>( this )->getPortOut();
}

template <typename T>
void PortBaseIn::setDefaultValue( const T& value ) {
    static_cast<PortIn<T>*>( this )->setDefaultValue( value );
}

template <typename T>
T& PortBaseIn::getData() {
    static_cast<PortIn<T>*>( this )->getData();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PortIn ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * PortIn is an Observable<PortIn<T>&> that notifies its observers at connect/disconnect event
 * @tparam T
 */
template <typename T>
PortIn<T>::PortIn( const std::string& name, Node* node ) : PortBaseIn( node, name, typeid( T ) ) {}

template <typename T>
PortIn<T>::PortIn( Node* node, const std::string& name ) : PortBaseIn( node, name, typeid( T ) ) {}

template <typename T>
PortBaseOut* PortIn<T>::getLink() {
    return m_from;
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
bool PortIn<T>::accept( PortOut<T>* portOut ) const {
    return !m_from;
}

template <typename T>
bool PortIn<T>::connect( PortBaseOut* portOut ) {
    if ( accept( portOut ) ) {
        m_from = static_cast<PortOut<T>*>( portOut );
        // notify after connect
        this->notify( getName(), *this, true );
    }
    return ( m_from );
}

template <typename T>
bool PortIn<T>::connect( PortOut<T>* portOut ) {
    if ( accept( portOut ) ) {
        m_from = portOut;
        // notify after connect
        this->notify( getName(), *this, true );
    }
    return ( m_from );
}

template <typename T>
bool PortIn<T>::disconnect() {
    if ( m_from ) {
        // notify before disconnect
        this->notify( getName(), *this, false );
        m_from = nullptr;
        return true;
    }
    return false;
}
template <typename T>
PortBaseOut* PortIn<T>::reflect( Node* node, std::string name ) const {
    return new PortOut<DataType>( name, node );
}

template <typename T>
bool PortIn<T>::is_input() {
    return true;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
