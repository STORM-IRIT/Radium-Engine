#include <Core/Utils/BijectiveAssociation.hpp>
namespace Ra {
namespace Core {
namespace Utils {

template <typename T1, typename T2>
BijectiveAssociation<T1, T2>::BijectiveAssociation(
    std::initializer_list<std::pair<key_type, value_type>> pairs ) {
    for ( auto& p : pairs ) {
        m_ValueToKey.insert( { p.second, p.first } );
        m_TKeyToValue.insert( p );
    }
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::addAssociation( std::pair<key_type, value_type> p ) {
    auto [i1, r1] = m_ValueToKey.insert( { p.second, p.first } );
    if ( r1 ) {
        auto [i2, r2] = m_TKeyToValue.insert( std::move( p ) );
        return r2;
    }
    return false;
}

template <typename T1, typename T2>
bool BijectiveAssociation<T1, T2>::addAssociation( key_type v1, value_type v2 ) {
    return addAssociation( { v1, v2 } );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::value_type
BijectiveAssociation<T1, T2>::operator()( const key_type& k ) const {
    return m_TKeyToValue.at( k );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::key_type
BijectiveAssociation<T1, T2>::key( const value_type& k ) const {
    return m_ValueToKey.at( k );
}

template <typename T1, typename T2>
typename BijectiveAssociation<T1, T2>::value_type
BijectiveAssociation<T1, T2>::value( const key_type& k ) const {
    return m_TKeyToValue.at( k );
}

} // namespace Utils
} // namespace Core
} // namespace Ra
