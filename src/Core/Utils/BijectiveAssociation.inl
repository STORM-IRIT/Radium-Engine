#pragma once
#include <Core/Utils/BijectiveAssociation.hpp>
namespace Ra {
namespace Core {
namespace Utils {

template <typename T1, typename T2>
BijectiveAssociation<T1, T2>::BijectiveAssociation(
    std::initializer_list<std::pair<key_type, value_type>> pairs ) {
    for ( auto& p : pairs ) {
        m_ValueToKey.insert( { p.second, p.first } );
        m_KeyToValue.insert( p );
    }
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::insert( std::pair<key_type, value_type> p ) {
    auto [i1, r1] = m_ValueToKey.insert( { p.second, p.first } );
    if ( r1 ) {
        auto [i2, r2] = m_KeyToValue.insert( std::move( p ) );
        return r2;
    }
    return false;
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::insert( key_type v1, value_type v2 ) {
    return insert( { v1, v2 } );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::value_type
BijectiveAssociation<T1, T2>::operator()( const key_type& k ) const {
    return m_KeyToValue.at( k );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::key_type
BijectiveAssociation<T1, T2>::key( const value_type& k ) const {
    return m_ValueToKey.at( k );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::value_type
BijectiveAssociation<T1, T2>::value( const key_type& k ) const {
    return m_KeyToValue.at( k );
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::cbegin() const noexcept {
    return m_KeyToValue.cbegin();
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::cend() const noexcept {
    return m_KeyToValue.cend();
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::begin() const noexcept {
    return m_KeyToValue.begin();
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::end() const noexcept {
    return m_KeyToValue.end();
}

template <typename T1, typename T2>
size_t BijectiveAssociation<T1, T2>::size() const {
    return m_KeyToValue.size();
}

} // namespace Utils
} // namespace Core
} // namespace Ra
