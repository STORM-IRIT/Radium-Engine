#include <Core/Mesh/DCEL/Iterator/Vertex/VertexIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template < typename OBJECT >
VIterator< OBJECT >::VIterator( Vertex_ptr& v ) :
    Iterator< OBJECT >(),
    m_v( v )
{
    this->m_he = m_v->HE();
}






/// DESTRUCTOR
template < typename OBJECT >
VIterator< OBJECT >::~VIterator() { }






/// SIZE
template < typename OBJECT >
inline uint VIterator< OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = m_v->HE();
    do {
        it = it->Prev()->Twin();
        ++i;
    } while( it != m_v->HE() );
    return i;
}






/// RESET
template < typename OBJECT >
inline void VIterator< OBJECT >::reset() {
    this->m_he = m_v->HE();
}






/// OPERATOR
template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator=( const VIterator& it ) {
    m_v = it.m_v;
    this->m_he = it.m_he;
    return *this;
}



template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator++() {
    this->m_he = this->m_he->Prev()->Twin();
    return *this;
}



template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator--() {
    this->m_he = this->m_he->Twin()->Next();
    return *this;
}



template < typename OBJECT >
inline bool VIterator< OBJECT >::operator==( const VIterator& it ) const {
    return ( ( m_v == it.m_v ) && ( this->m_he == it.m_he ) );
}



} // namespace Core
} // namespace Ra
