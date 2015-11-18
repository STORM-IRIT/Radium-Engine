#include <Core/Mesh/DCEL/Iterator/Vertex/VFEIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
VFEIterator::VFEIterator( const Vertex& v ) : VIterator< FullEdge_ptr >( v ) { }



/// DESTRUCTOR
VFEIterator::~VFEIterator() { }



/// LIST
inline FullEdgeList VFEIterator::list() const {
    FullEdgeList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it->FE() );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}



/// OPERATOR
inline FullEdge_ptr VFEIterator::operator->() const {
    return m_he->FE();
}



} // namespace Core
} // namespace Ra
