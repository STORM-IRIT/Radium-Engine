#include <Core/Utils/Index.hpp>

namespace Ra {
namespace Core {
namespace Utils {
/// CONSTRUCTOR
constexpr Index::Index( IntegerType i ) : m_idx( ( i < 0 ) ? s_invalid : i ) {}

constexpr Index::Index( const Index& i ) : m_idx( i.m_idx ) {}

constexpr Index& Index::operator=( long int i ) {
    m_idx = ( i < 0 ) ? s_invalid : i;
    return *this;
}
/// VALID
constexpr bool Index::isValid() const {
    return ( m_idx != s_invalid );
}

/// INVALID
constexpr bool Index::isInvalid() const {
    return ( m_idx < 0 );
}
constexpr void Index::setInvalid() {
    m_idx = s_invalid;
}
constexpr Index Index::Invalid() {
    return Index( s_invalid );
}
constexpr Index Index::Max() {
    return Index( s_maxIdx );
}

/// INDEX
constexpr Index::IntegerType Index::getValue() const {
    return m_idx;
}
constexpr void Index::setValue( const IntegerType i ) {
    m_idx = ( i < 0 ) ? s_invalid : i;
}

/// CAST
constexpr Index::operator int() const {
    return m_idx;
}

/// OPERATOR
constexpr Index& Index::operator=( const Index& id ) {
    m_idx = id.m_idx;
    return *this;
}
constexpr Index& Index::operator++() {
    m_idx++;
    if ( m_idx < 0 ) { m_idx = s_invalid; }
    return *this;
}
constexpr Index& Index::operator--() {
    if ( m_idx != s_invalid ) { m_idx--; }
    return *this;
}

constexpr Index Index::operator+( const Index& id ) {
    if ( isInvalid() || id.isInvalid() ) { return Index::Invalid(); }
    return Index( m_idx + id.m_idx );
}
template <typename Integer>
constexpr Index Index::operator+( const Integer& id ) {
    return ( *this ) + Index( id );
}

constexpr Index Index::operator-( const Index& id ) {
    if ( isInvalid() || id.isInvalid() ) { return Index::Invalid(); }
    return Index( m_idx - id.m_idx );
}
template <typename Integer>
constexpr Index Index::operator-( const Integer& id ) {
    return ( *this ) - Index( id );
}

constexpr bool Index::operator==( const Index& id ) {
    return ( m_idx == id.m_idx );
}
template <typename Integer>
constexpr bool Index::operator==( const Integer& i ) {
    return ( *this == Index( IntegerType( i ) ) );
}

constexpr bool Index::operator!=( const Index& id ) {
    return ( !( *this == id ) );
}
template <typename Integer>
constexpr bool Index::operator!=( const Integer& i ) {
    return ( !( *this == Index( IntegerType( i ) ) ) );
}

constexpr bool Index::operator<( const Index& id ) {
    if ( isInvalid() || id.isInvalid() ) { return false; }
    return ( m_idx < id.m_idx );
}
template <typename Integer>
constexpr bool Index::operator<( const Integer& i ) {
    return ( *this < Index( IntegerType( i ) ) );
}

constexpr bool Index::operator<=( const Index& id ) {
    if ( isInvalid() || id.isInvalid() ) { return false; }
    return ( m_idx <= id.m_idx );
}
template <typename Integer>
constexpr bool Index::operator<=( const Integer& i ) {
    return ( *this <= Index( IntegerType( i ) ) );
}

constexpr bool Index::operator>( const Index& id ) {
    if ( isInvalid() || id.isInvalid() ) { return false; }
    return ( m_idx > id.m_idx );
}
template <typename Integer>
constexpr bool Index::operator>( const Integer& i ) {
    return ( *this > Index( IntegerType( i ) ) );
}

constexpr bool Index::operator>=( const Index& id ) {
    if ( isInvalid() || id.isInvalid() ) { return false; }
    return ( m_idx >= id.m_idx );
}
template <typename Integer>
constexpr bool Index::operator>=( const Integer& i ) {
    return ( *this >= Index( IntegerType( i ) ) );
}

} // namespace Utils
} // namespace Core
} // namespace Ra
