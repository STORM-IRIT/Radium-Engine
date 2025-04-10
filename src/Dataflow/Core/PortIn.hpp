#pragma once

#include <Dataflow/RaDataflow.hpp>

#include <Core/Containers/VariableSet.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/Observable.hpp>
#include <Dataflow/Core/Port.hpp>

#include <optional>

namespace Ra {
namespace Dataflow {
namespace Core {

class PortBaseOut; /// Forward PortOut classes used by getLink and reflect
template <typename T>
class PortOut;

class RA_DATAFLOW_CORE_API PortBaseIn : public PortBase
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
    virtual bool isLinked() const                 = 0;
    virtual bool hasDefaultValue() const          = 0;
    virtual void insert( Ra::Core::VariableSet& ) = 0;
    virtual PortBaseOut* getLink()                = 0;

    template <typename T>
    PortOut<T>* getLinkAs();

    template <typename T>
    void setDefaultValue( const T& value );

    template <typename T>
    T& data();

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
    PortIn( Node* node, const std::string& name ) : PortBaseIn( node, name, typeid( T ) ) {
        add_port_type<T>();
    }
    PortIn( Node* node, const std::string& name, const T& value ) :
        PortBaseIn( node, name, typeid( T ) ), m_defaultValue { value } {
        add_port_type<T>();
    }
    /// @}

    /// Returns true if the port is linked to an output port that has data or if it has a default
    /// value.
    bool has_data() override;

    ///\brief Gets the data pointed by the connected out port.
    ///
    /// It checks if the data is available. If it is not, it returns the default value.
    ///
    ///\return T& The reference to the data.
    T& data();

    /// \name Manage the connection with a PortOut.
    /// @{
    bool accept( PortBaseOut* portOut ) const override;
    bool accept( PortOut<T>* portOut ) const;

    bool connect( PortBaseOut* portOut ) override;
    bool connect( PortOut<T>* portOut );

    bool disconnect() override;

    PortBaseOut* getLink() override { return m_from; }
    /// @}

    void setDefaultValue( const T& value ) { m_defaultValue = value; }
    T& getDefaultValue() { return *m_defaultValue; }
    bool hasDefaultValue() const override { return m_defaultValue.has_value(); }
    void insert( Ra::Core::VariableSet& v ) override {
        if ( hasDefaultValue() ) v.setVariable( name(), std::reference_wrapper( *m_defaultValue ) );
    }

    bool isLinked() const override { return m_from != nullptr; }

    template <typename B                                                              = T,
              std::enable_if_t<std::is_constructible<nlohmann::json, B>::value, bool> = true>
    void to_json_impl( nlohmann::json& data ) {
        if ( hasDefaultValue() ) { data["default_value"] = getDefaultValue(); }
    }
    template <typename B                                                               = T,
              std::enable_if_t<!std::is_constructible<nlohmann::json, B>::value, bool> = true>
    void to_json_impl( nlohmann::json& data ) {
        if ( hasDefaultValue() ) {
            data["default_value"] =
                std::string( "Default value not saved, missing json export for " ) +
                Ra::Core::Utils::simplifiedDemangledType<T>();
        }
    }
    template <typename B                                                           = T,
              std::enable_if_t<std::is_assignable<nlohmann::json, B>::value, bool> = true>
    void from_json_impl( const nlohmann::json& data ) {
        using namespace Ra::Core::Utils;
        if ( auto value_it = data.find( "default_value" ); value_it != data.end() ) {
            setDefaultValue( ( *value_it ).template get<T>() );
        }
    }
    template <typename B                                                           = T,
              std::enable_if_t<!std::is_assignable<nlohmann::json, B>::value, int> = true>
    void from_json_impl( const nlohmann::json& data ) {
        if ( auto it = data.find( "name" ); it != data.end() ) { set_name( *it ); }
    }
    void to_json( nlohmann::json& data ) override {
        PortBase::to_json( data );
        to_json_impl( data );
    }
    void from_json( const nlohmann::json& data ) override {
        PortBase::from_json( data );
        from_json_impl( data );
    }

  private:
    PortOut<T>* m_from = nullptr;       ///< A pointer to the out port this port is connected to.
    std::optional<T> m_defaultValue {}; ///< The value used when not connected.

}; // class PortIn<T>

template <typename Type>
using PortInPtr = PortPtr<PortIn<Type>>;
template <typename Type>
using PortInRawPtr = typename PortInPtr<Type>::element_type*;

using PortBaseInPtr    = PortPtr<PortBaseIn>;
using PortBaseInRawPtr = PortRawPtr<PortBaseIn>;

template <typename T>
PortOut<T>* PortBaseIn::getLinkAs() {
    return static_cast<PortIn<T>*>( this )->getLink();
}

template <typename T>
void PortBaseIn::setDefaultValue( const T& value ) {
    static_cast<PortIn<T>*>( this )->setDefaultValue( value );
}

template <typename T>
T& PortBaseIn::data() {
    return static_cast<PortIn<T>*>( this )->data();
}

template <typename T>
T& PortIn<T>::data() {
    if ( isLinked() ) return m_from->data();
    if ( m_defaultValue ) return *m_defaultValue;
    CORE_ASSERT( false, "should not get here" );
    using namespace Ra::Core::Utils;
    LOG( logERROR ) << "graph is not valid";
    return *m_defaultValue;
}

template <typename T>
inline bool PortIn<T>::has_data() {
    if ( isLinked() ) { return m_from->has_data(); }
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
        this->notify( name(), *this, true );
    }
    return ( m_from );
}

template <typename T>
bool PortIn<T>::connect( PortOut<T>* portOut ) {
    if ( accept( portOut ) ) {
        m_from = portOut;
        m_from->increaseLinkCount();
        // notify after connect
        this->notify( name(), *this, true );
    }
    return ( m_from );
}

template <typename T>
bool PortIn<T>::disconnect() {
    if ( m_from ) {
        // notify before disconnect
        m_from->decreaseLinkCount();
        this->notify( name(), *this, false );
        m_from = nullptr;
        return true;
    }
    return false;
}

} // namespace Core
} // namespace Dataflow
} // namespace Ra
