#include <Core/Utils/BijectiveAssociation.hpp>
namespace Ra {
namespace Core {
namespace Utils {

template <typename T1, typename T2>
BijectiveAssociation<T1, T2>::BijectiveAssociation(
    std::initializer_list<std::pair<T1, T2>> pairs ) {
    for ( auto& p : pairs ) {
        m_Type2ToType1.insert( { p.second, p.first } );
        m_Type1ToType2.insert( p );
    }
}

template <typename T1, typename T2>
BijectiveAssociation<T1, T2>::BijectiveAssociation(
    std::initializer_list<std::pair<T2, T1>> pairs ) {
    for ( auto& p : pairs ) {
        m_Type1ToType2.insert( { p.second, p.first } );
        m_Type2ToType1.insert( p );
    }
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::addAssociation( std::pair<T1, T2> p ) {
    auto [i1, r1] = m_Type2ToType1.insert( { p.second, p.first } );
    if ( r1 ) {
        auto [i2, r2] = m_Type1ToType2.insert( std::move( p ) );
        return r2;
    }
    return false;
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::addAssociation( std::pair<T2, T1> p ) {
    auto [i1, r1] = m_Type1ToType2.insert( { p.second, p.first } );
    if ( r1 ) {
        auto [i2, r2] = m_Type2ToType1.insert( std::move( p ) );
        return r2;
    }
    return false;
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::addAssociation( T1 v1, T2 v2 ) {
    return addAssociation( { v1, v2 } );
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::addAssociation( T2 v2, T1 v1 ) {
    return addAssociation( { v2, v1 } );
}

template <typename T1, typename T2>
T1 BijectiveAssociation<T1, T2>::operator()( const T2& k ) const {
    return m_Type2ToType1.at( k );
}

template <typename T1, typename T2>
T2 BijectiveAssociation<T1, T2>::operator()( const T1& k ) const {
    return m_Type1ToType2.at( k );
}

} // namespace Utils
} // namespace Core
} // namespace Ra
