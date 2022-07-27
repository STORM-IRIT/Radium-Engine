#pragma once

#include <initializer_list>
#include <map>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * \brief Bijective association on a finite set of <key, value> pairs.
 * \tparam T1 Type of one element of the pair
 * \tparam T2 Type of the other element of the pair
 * \note T1 must be different from T2
 */
template <typename T1, typename T2>
class BijectiveAssociation
{
  public:
    using Type1 = T1;
    using Type2 = T2;

    /**
     * \brief Constructor from { <T1, T2> } pairs
     */
    explicit BijectiveAssociation( std::initializer_list<std::pair<T1, T2>> pairs );
    /**
     * \brief Constructor from { <T2, T1> } pairs
     */
    explicit BijectiveAssociation( std::initializer_list<std::pair<T2, T1>> pairs );

    /**
     * \brief Add a pair to the relation.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool addAssociation( std::pair<Type1, Type2> p );

    /**
     * \brief Add a pair to the relation.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool addAssociation( std::pair<Type2, Type1> p );

    /**
     * \brief Add a pair to the relation.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool addAssociation( Type1 v1, Type2 v2 );

    /**
     * \brief Add a pair to the relation.
     * \return false if the pair was not added due to collision with an already inserted pair
     */
    bool addAssociation( Type2 v2, Type1 v1 );

    /**
     * \brief Gets the value associated to the key
     */
    Type1 operator()( const Type2& k ) const;

    /**
     * \brief Gets the value associated to the key
     */
    Type2 operator()( const Type1& k ) const;

  private:
    std::map<Type1, Type2> m_Type1ToType2;
    std::map<Type2, Type1> m_Type2ToType1;
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/BijectiveAssociation.inl>
