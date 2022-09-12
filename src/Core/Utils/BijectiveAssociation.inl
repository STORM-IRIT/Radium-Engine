#pragma once
#include <Core/Utils/BijectiveAssociation.hpp>
#include <algorithm>
#include <iostream>
namespace Ra {
namespace Core {
namespace Utils {

template <typename T1, typename T2>
BijectiveAssociation<T1, T2>::BijectiveAssociation(
    std::initializer_list<std::pair<key_type, value_type>> pairs ) {
    for ( auto& p : pairs ) {
        m_valueToKey.insert( { p.second, p.first } );
        m_keyToValue.insert( p );
    }
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::insert( std::pair<key_type, value_type> p ) {
    auto [i1, r1] = m_valueToKey.insert( { p.second, p.first } );
    if ( r1 ) {
        auto [i2, r2] = m_keyToValue.insert( std::move( p ) );
        return r2;
    }
    return false;
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::insert( key_type key, value_type value ) {
    return insert( { key, value } );
}

template <typename T1, typename T2>
void BijectiveAssociation<T1, T2>::replace( key_type key, value_type value ) {
    // clean previously set association
    auto it1 = std::find_if(
        m_keyToValue.begin(),
        m_keyToValue.end(),
        [&value]( const typename key_to_value_map::value_type& v ) { return value == v.second; } );

    if ( it1 != m_keyToValue.end() ) m_keyToValue.erase( it1 );

    auto it2 = std::find_if(
        m_valueToKey.begin(),
        m_valueToKey.end(),
        [&key]( const typename value_to_key_map::value_type& v ) { return key == v.second; } );

    if ( it2 != m_valueToKey.end() ) m_valueToKey.erase( it2 );

    // set new association
    m_keyToValue[key]   = value;
    m_valueToKey[value] = key;
}
template <typename T1, typename T2>
void BijectiveAssociation<T1, T2>::replace( std::pair<key_type, value_type> p ) {
    replace( p.first, p.second );
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::remove( key_type key, value_type value ) {
    // clean previously set association
    auto it1 = m_keyToValue.find( key );
    auto it2 = m_valueToKey.find( value );
    if ( it1 == m_valueToKey.end() || it2 == m_keyToValue.end() ) return false;

    if ( it1->second != value || it2->second != key ) return false;

    m_keyToValue.erase( it1 );
    m_valueToKey.erase( it2 );
    return true;
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::remove( std::pair<key_type, value_type> p ) {
    return remove( p.frist, p.second );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::value_type
BijectiveAssociation<T1, T2>::operator()( const key_type& k ) const {
    return m_keyToValue.at( k );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::key_type
BijectiveAssociation<T1, T2>::key( const value_type& k ) const {
    return m_valueToKey.at( k );
}

template <typename T1, typename T2>
std::optional<typename BijectiveAssociation<T1, T2>::key_type>
BijectiveAssociation<T1, T2>::keyIfExists( const value_type& k ) const {
    auto itr = m_valueToKey.find( k );
    if ( itr == m_valueToKey.end() ) return {};
    return itr->second;
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::value_type
BijectiveAssociation<T1, T2>::value( const key_type& k ) const {
    return m_keyToValue.at( k );
}

template <typename T1, typename T2>
std::optional<typename BijectiveAssociation<T1, T2>::value_type>
BijectiveAssociation<T1, T2>::valueIfExists( const key_type& k ) const {
    auto itr = m_keyToValue.find( k );
    if ( itr == m_keyToValue.end() ) return {};
    return itr->second;
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::cbegin() const noexcept {
    return m_keyToValue.cbegin();
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::cend() const noexcept {
    return m_keyToValue.cend();
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::begin() const noexcept {
    return m_keyToValue.begin();
}

template <typename T1, typename T2>
typename std::map<T1, T2>::const_iterator BijectiveAssociation<T1, T2>::end() const noexcept {
    return m_keyToValue.end();
}

template <typename T1, typename T2>
size_t BijectiveAssociation<T1, T2>::size() const {
    return m_keyToValue.size();
}

} // namespace Utils
} // namespace Core
} // namespace Ra
