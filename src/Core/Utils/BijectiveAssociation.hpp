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
    using key_type   = T1;
    using value_type = T2;

    /**
     * \brief Constructor from { <T1, T2> } pairs
     */
    explicit BijectiveAssociation( std::initializer_list<std::pair<key_type, value_type>> pairs );

    /**
     * \brief Add a pair to the relation.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool addAssociation( std::pair<key_type, value_type> p );

    /**
     * \brief Add a pair to the relation.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool addAssociation( key_type v1, value_type v2 );

    /**
     * \brief Gets the value associated to the key
     */
    value_type operator()( const key_type& k ) const;

    /**
     * \brief Gets the value associated to the key
     */
    value_type value( const key_type& k ) const;

    /**
     * \brief Gets the key  associated to the value
     */
    key_type key( const value_type& k ) const;

  private:
    std::map<key_type, value_type> m_TKeyToValue;
    std::map<value_type, key_type> m_ValueToKey;
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/BijectiveAssociation.inl>
