#pragma once

#include <initializer_list>
#include <map>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * \brief Bijective association on a finite set of <key, value> pairs.
 * \tparam T1 Type of the key
 * \tparam T2 Type of the value
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
    explicit BijectiveAssociation() = default;

    /**
     * \brief Add a pair to the association.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool insert( std::pair<key_type, value_type> p );
    /**
     * \brief Convenient alias for element insertion (hides pair creation).

Code is equivalent to `insert({v1, v2})`.
     * \see Ra::Core::Utils::BijectiveAssociation::insert(std::pair<key_type, value_type>)
     */
    bool insert( key_type v1, value_type v2 );

    /**
     * \brief Replace a pair in the association.
     *
     * Remove the association {p.first, old_value} and {old_key, p.second} and insert {p.first,
     * p.second} Equivalent to \code{.cpp} remove({p.first, old_value}); remove({old_key,
     * p.second}); insert(p); \endcode But faster
     */
    void replace( std::pair<key_type, value_type> p );

    /**
     * \brief Convenient alias for replace({v1, v2}).
     * \sa Ra::Core::Utils::BijectiveAssociation::replace(std::pair<key_type, value_type>)
     */
    void replace( key_type v1, value_type v2 );

    /**
     * \brief Remove a pair in the association.
     */
    void remove( std::pair<key_type, value_type> p );
    /**
     * \brief Convenient alias for remove({v1, v2});
     * \sa Ra::Core::Utils::BijectiveAssociation::remove(std::pair<key_type, value_type>)
     */
    void remove( key_type v1, value_type v2 );

    /**
     * \brief Gets the value associated to the key
     */
    value_type operator()( const key_type& k ) const;

    /**
     * \brief Gets the value associated to the key.
     * \throw std::out_of_range if k is not present in the association as a key.
     */
    value_type value( const key_type& k ) const;

    /**
     * \brief Gets the value associated to a key if it exists.
     *
     * Otherwise the optional doesn't contain a value, and evaluate to false.
     */
    std::optional<value_type> valueIfExists( const key_type& k ) const;

    /**
     * \brief Gets the key associated to a value.
     * \throw std::out_of_range if k is not present in the association as a value.
     */
    key_type key( const value_type& k ) const;

    /**
     * \brief Gets the key associated to a value if it exists.
     *
     * Otherwise the optional doesn't contain a value, and evaluate to false.
     */
    std::optional<key_type> keyIfExists( const value_type& k ) const;

    /**
     * \brief Gets an const iterator at beginning of the key to value map.
     */
    typename std::map<key_type, value_type>::const_iterator begin() const noexcept;
    /// \copydoc begin()
    typename std::map<key_type, value_type>::const_iterator cbegin() const noexcept;

    /**
     * \brief Gets a const iterator at the end of the key to value map.
     */
    typename std::map<key_type, value_type>::const_iterator end() const noexcept;
    ///\copydoc end()
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

#include <Core/Utils/BijectiveAssociation.inl>
