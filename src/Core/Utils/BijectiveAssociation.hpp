#pragma once

#include <initializer_list>
#include <map>

#include <Core/Utils/StdOptional.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * \brief Bijective association between two sets {keys} and {values} having the same cardinality.
 * The bijection \f$f\f$ is constructed by giving all the pairs <key, value> such that \f$f(key) =
 * value\f$. \tparam T1 Type of the keys \tparam T2 Type of the values
 */
template <typename T1, typename T2>
class BijectiveAssociation
{
  public:
    using key_type         = T1;
    using value_type       = T2;
    using key_to_value_map = std::map<key_type, value_type>;
    using value_to_key_map = std::map<value_type, key_type>;

    /**
     * \brief Constructor from { <T1, T2> } pairs
     */
    explicit BijectiveAssociation( std::initializer_list<std::pair<key_type, value_type>> pairs );

    /**
     * \brief Creates an empty association.
     */
    explicit BijectiveAssociation() = default;

    /**
     * \brief Insert a pair into the association.
     * \return false if the pair was not added due to collision with an already inserted pair.
     */
    bool insert( std::pair<key_type, value_type> p );

    /**
     * \brief Convenient alias for `insert({key, value})`.
     * \see Ra::Core::Utils::BijectiveAssociation::insert(std::pair<key_type, value_type>)
     */
    bool insert( key_type key, value_type value );

    /**
     * \brief Replace a pair in the association.
     *
     * Remove the associations `{p.first, old_value}` and `{old_key, p.second}` and insert
`{p.first,
     * p.second}`.
     * Faster version of
     * \code{.cpp}
     * remove({p.first, old_value});
     * remove({old_key, p.second});
     * insert(p);
     * \endcode
     */
    void replace( std::pair<key_type, value_type> p );

    /**
     * \brief Convenient alias of `replace({key, value})`.
     * \see Ra::Core::Utils::BijectiveAssociation::replace(std::pair<key_type, value_type>)
     */
    void replace( key_type key, value_type value );

    /**
     * \brief Remove a pair from the association.
     *
     * Remove the `{key, value}` pair. If \b key (resp. \b value) is present but associated with a
     * value (resp. key)
     * != \b value (resp. \b key), the association stay unchanged.
     * \return true if the pair was in the association, and removed. False otherwise.
     */
    bool remove( std::pair<key_type, value_type> p );

    /**
     * \brief Convenient alias for `remove({key, value})`.
     * \see Ra::Core::Utils::BijectiveAssociation::remove(std::pair<key_type, value_type>)
     */
    bool remove( key_type key, value_type value );

    /**
     * \brief Gets the value associated to the key
     * \throw std::out_of_range if \b key is not present in the association as a key.
     */
    value_type operator()( const key_type& key ) const;

    /**
     * \brief Gets the value associated to \b key.
     * \throw std::out_of_range if \b key is not present in the association as a key.
     */
    value_type value( const key_type& key ) const;

    /**
     * \brief Gets the value associated to \b key if it exists.
     *
     * Otherwise the optional doesn't contain anything, and evaluate to false.
     */
    optional<value_type> valueIfExists( const key_type& key ) const;

    /**
     * \brief Gets the key associated to a value.
     * \throw std::out_of_range if \b value is not present in the association as a value.
     */
    key_type key( const value_type& value ) const;

    /**
     * \brief Gets the key associated to \b value if it exists.
     *
     * Otherwise the optional doesn't contain anything, and evaluate to false.
     */
    optional<key_type> keyIfExists( const value_type& value ) const;

    /**
     * \brief Gets a const iterator at beginning of the key to value map.
     */
    typename std::map<key_type, value_type>::const_iterator begin() const noexcept;

    /// \copydoc begin()
    typename std::map<key_type, value_type>::const_iterator cbegin() const noexcept;

    /**
     * \brief Gets a const iterator at the end of the key to value map.
     */
    typename std::map<key_type, value_type>::const_iterator end() const noexcept;

    /// \copydoc end()
    typename std::map<key_type, value_type>::const_iterator cend() const noexcept;

    /**
     * \brief Gets the size of the association, i.e. the numer of <Key, Value> pairs.
     */
    size_t size() const;

  private:
    key_to_value_map m_keyToValue;
    value_to_key_map m_valueToKey;
};

} // namespace Utils
} // namespace Core
} // namespace Ra

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
    if ( it1 == m_keyToValue.end() || it2 == m_valueToKey.end() ) return false;

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
