#pragma once

#include <Core/CoreMacros.hpp>

#include <Core/Utils/BijectiveAssociation.hpp>

#include <initializer_list>
#include <vector>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * \brief This class manage the bijective association between string and integral representation
 * of an enumeration.
 *
 * \tparam EnumBaseType the underlying type (as returned by typename std::underlying_type_t<Enum>)
 * of the enumeration to manage.
 *
 */

template <typename EnumBaseType>
class EnumConverter
{
    /// \todo think of manage both EnumType and EnumUnderlyingType
    /// Eg using EnumUnderlyingType = std::underling_type_t<Enum>;
    /// getEnumeratorUnderlying ->EnumUnderlyingType
    /// getEnumerator -> Enum
    /// After thinking, complexify management of variableset and visitor.
  public:
    explicit EnumConverter( std::initializer_list<std::pair<EnumBaseType, std::string>> pairs ) :
        m_valueToString { pairs } {}

    std::string getEnumerator( EnumBaseType v ) const { return m_valueToString( v ); }
    EnumBaseType getEnumerator( const std::string& v ) const { return m_valueToString.key( v ); }
    std::vector<std::string> getEnumerators() const {
        std::vector<std::string> keys;
        keys.reserve( m_valueToString.size() );
        for ( const auto& p : m_valueToString ) {
            keys.push_back( p.second );
        }
        return keys;
    }

  private:
    Core::Utils::BijectiveAssociation<EnumBaseType, std::string> m_valueToString;
};

} // namespace Utils
} // namespace Core
} // namespace Ra
