#include <Core/Mesh/DCEL/Iterator/Face/FHEIterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
FHEIterator::FHEIterator( const Face& f ) : FIterator< HalfEdge_ptr >( f ) { }



/// DESTRUCTOR
FHEIterator::~FHEIterator() { }



/// LIST
inline HalfEdgeList FHEIterator::list() const {
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



} // namespace Core
} // namespace Ra
