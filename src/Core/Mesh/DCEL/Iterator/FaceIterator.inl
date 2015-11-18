#include <Core/Mesh/DCEL/Iterator/FaceIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template < typename OBJECT >
FIterator< OBJECT >::FIterator( const Face& f ) :
    Iterator< OBJECT >(),
    m_f( &f ) {
    m_he = m_f->HE();
}






/// DESTRUCTOR
template < typename OBJECT >
FIterator< OBJECT >::~FIterator() { }






/// SIZE
template < typename OBJECT >
inline uint FIterator< OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = m_f->HE();
    do {
        it = it->Next();
        ++i;
    } while( it != m_f->HE() );
    return i;
}






/// RESET
template < typename OBJECT >
inline void FIterator< OBJECT >::reset() {
    m_he = m_f->HE();
}






/// OPERATOR
template < typename OBJECT >
inline FIterator< OBJECT >& FIterator< OBJECT >::operator=( const VIterator& it ) {
    m_v = it.m_v;
    m_he = it.m_he;
    return *this;
}



template < typename OBJECT >
inline FIterator< OBJECT >& FIterator< OBJECT >::operator++() {
    m_he = m_he->Next();
    return *this;
}



template < typename OBJECT >
inline FIterator< OBJECT >& FIterator< OBJECT >::operator--() {
    m_he = m_he->Prev();
    return *this;
}



template < typename OBJECT >
inline bool FIterator< OBJECT >::operator==( const FIterator& it ) const {
    return ( ( m_f == it.m_f ) && ( m_he == it.m_he ) );
}



//======================================================================================

/// CONSTRUCTOR
FVIterator::FVIterator( const Face& f ) : FIterator< Vertex_ptr >( f ) { }



/// DESTRUCTOR
FVIterator::~FVIterator() { }



/// LIST
inline VertexList FVIterator::list() const {
    VertexList L;
    HalfEdge_ptr it = m_f->HE();
    do {
        L.push_back( it->V() );
        it = it->Next();
    } while( it != m_f->HE() );
    return L;
}



/// OPERATOR
inline Vertex_ptr FVIterator::operator->() const {
    return m_he->V();
}

//======================================================================================

/// CONSTRUCTOR
FHEIterator::FHEIterator( const Face& f ) : FIterator< HalfEdge_ptr >( f ) { }



/// DESTRUCTOR
FHEIterator::~FHEIterator() { }



/// LIST
inline HalfEdgeList FVIterator::list() const {
    HalfEdgeList L;
    HalfEdge_ptr it = m_f->HE();
    do {
        L.push_back( it );
        it = it->Next();
    } while( it != m_f->HE() );
    return L;
}



/// OPERATOR
inline HalfEdge_ptr FHEIterator::operator->() const {
    return m_he;
}

//======================================================================================

/// CONSTRUCTOR
FFIterator::FFIterator( const Face& f ) : FIterator< Face_ptr >( f ) { }



/// DESTRUCTOR
FFIterator::~FFIterator() { }



/// LIST
inline FaceList FVIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_f->HE();
    do {
        L.push_back( it->F() );
        it = it->Next();
    } while( it != m_f->HE() );
    return L;
}



/// OPERATOR
inline Face_ptr FFIterator::operator->() const {
    return m_he->Twin()->F();
}

} // namespace Core
} // namespace Ra
