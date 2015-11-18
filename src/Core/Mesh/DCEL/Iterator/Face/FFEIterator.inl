#include <Core/Mesh/DCEL/Iterator/Face/FFEIterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
FFEIterator::FFEIterator( const Face& f ) : FIterator< FullEdge_ptr >( f ) { }



/// DESTRUCTOR
FFEIterator::~FFEIterator() { }



/// LIST
inline HalfEdgeList FFEIterator::list() const {
    FullEdgeList L;
    HalfEdge_ptr it = m_f->HE();
    do {
        L.push_back( it->FE() );
        it = it->Next();
    } while( it != m_f->HE() );
    return L;
}



/// OPERATOR
inline HalfEdge_ptr FFEIterator::operator->() const {
    return m_he->FE();
}



} // namespace Core
} // namespace Ra
