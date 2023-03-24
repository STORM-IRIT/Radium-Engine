#pragma once
#include <Core/CoreMacros.hpp>

#include <initializer_list>

#include <Core/Utils/BijectiveAssociation.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * \brief This class manage the bijective association between string and integral representation
 * of an enumeration.
 *
 * \tparam EnumBaseType the underlying type (as returned by typename std::underlying_type_t<Enum>)
 * of the enumeration to manage
 *
 * Management of string representation of enum type.
 * \todo, reword the following
 * This allow to set the parameter using a string representation of their value.
 * Setting the parameter directly from the value is supported as for any other parameter but
 * user should take care to call the right overloaded function given the underlying enumeration
 * type. This is due to unscoped enum being implicitly convertible to any integral type.
 * (https://en.cppreference.com/w/cpp/language/enum)
 */
template <typename EnumBaseType>
class EnumConverter
{
  public:
    explicit EnumConverter( std::initializer_list<std::pair<EnumBaseType, std::string>> pairs );

    std::string getEnumerator( EnumBaseType v ) const;
    EnumBaseType getEnumerator( const std::string& v ) const;
    std::vector<std::string> getEnumerators() const;

  private:
    Core::Utils::BijectiveAssociation<EnumBaseType, std::string> m_valueToString;
};

/* -------------------------------------------------------------------------------------------
 * Implementation of template functions
 * -----------------------------------------------------------------------------------------*/

template <typename EnumBaseType>
EnumConverter<EnumBaseType>::EnumConverter(
    std::initializer_list<std::pair<EnumBaseType, std::string>> pairs ) :
    m_valueToString { pairs } {}

template <typename EnumBaseType>
std::string EnumConverter<EnumBaseType>::getEnumerator( EnumBaseType v ) const {
    return m_valueToString( v );
}

template <typename EnumBaseType>
EnumBaseType EnumConverter<EnumBaseType>::getEnumerator( const std::string& v ) const {
    return m_valueToString.key( v );
}

template <typename EnumBaseType>
std::vector<std::string> EnumConverter<EnumBaseType>::getEnumerators() const {
    std::vector<std::string> keys;
    keys.reserve( m_valueToString.size() );
    for ( const auto& p : m_valueToString ) {
        keys.push_back( p.second );
    }
    return keys;
}

} // namespace Utils
} // namespace Core
} // namespace Ra
