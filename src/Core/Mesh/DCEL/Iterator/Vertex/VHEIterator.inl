#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

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



} // namespace Core
} // namespace Ra
