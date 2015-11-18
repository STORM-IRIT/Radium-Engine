#include <Core/Mesh/DCEL/Iterator/Iterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
template < typename OBJECT >
Iterator< OBJECT >::Iterator() : m_he( nullptr ) { }






/// DESTRUCTOR
template < typename OBJECT >
Iterator< OBJECT >::~Iterator() { }






/// OPERATOR
template < typename OBJECT >
inline Iterator< OBJECT >& Iterator< OBJECT >::operator+=( const uint n ) const {
    for( uint i = 0; i < n; ++i ) {
        ++*this;
    }
    return *this;
}



template < typename OBJECT >
inline Iterator< OBJECT >& Iterator< OBJECT >::operator-=( const uint n ) const {
    for( uint i = 0; i < n; ++i ) {
        --*this;
    }
    return *this;
}



template < typename OBJECT >
inline Iterator< OBJECT > Iterator< OBJECT >::operator+ ( const uint n ) const {
    Iterator it = *this;
    it += n;
    return it;
}



template < typename OBJECT >
inline Iterator< OBJECT > Iterator< OBJECT >::operator- ( const uint n ) const {
    Iterator it = *this;
    it -= n;
    return it;
}



} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_DCEL_ITERATOR_HPP
