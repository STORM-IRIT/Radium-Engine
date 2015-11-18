#include <Core/Mesh/DCEL/Iterator/VertexIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template < typename OBJECT >
VIterator< OBJECT >::VIterator( const Vertex& v ) :
    Iterator< OBJECT >(),
    m_v( &v ) {
    m_he = m_v->HE();
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
    m_he = m_v->HE();
}






/// OPERATOR
template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator=( const VIterator& it ) {
    m_v = it.m_v;
    m_he = it.m_he;
    return *this;
}



template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator++() {
    m_he = m_he->Prev()->Twin();
    return *this;
}



template < typename OBJECT >
inline VIterator< OBJECT >& VIterator< OBJECT >::operator--() {
    m_he = m_he->Twin()->Next();
    return *this;
}



template < typename OBJECT >
inline bool VIterator< OBJECT >::operator==( const VIterator& it ) const {
    return ( ( m_v == it.m_v ) && ( m_he == it.m_he ) );
}


//======================================================================================

/// CONSTRUCTOR
VVIterator::VVIterator( const Vertex& v ) : VIterator< Vertex_ptr >( v ) { }



/// DESTRUCTOR
VVIterator::~VVIterator() { }



/// LIST
inline VertexList VVIterator::list() const {
    VertexList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it->Next()->V() );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}



/// OPERATOR
inline Vertex_ptr VVIterator::operator->() const {
    return m_he->Next()->V();
}

//======================================================================================

/// CONSTRUCTOR
VHEIterator::VHEIterator( const Vertex& v ) : VIterator< HalfEdge_ptr >( v ) { }



/// DESTRUCTOR
VHEIterator::~VHEIterator() { }



/// LIST
inline HalfEdgeList VHEIterator::list() const {
    HalfEdgeList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}



/// OPERATOR
inline HalfEdge_ptr VHEIterator::operator->() const {
    return m_he;
}

//======================================================================================

/// CONSTRUCTOR
VFIterator::VFIterator( const Vertex& v ) : VIterator< Face_ptr >( v ) { }



/// DESTRUCTOR
VFIterator::~VFIterator() { }



/// LIST
inline FaceList VFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it->F() );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}



/// OPERATOR
inline Face_ptr VFIterator::operator->() const {
    return m_he->F();
}



} // namespace Core
} // namespace Ra
