#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VHEIterator::VHEIterator( Vertex_ptr& v ) : VIterator< HalfEdge >( v ) { }



/// DESTRUCTOR
VHEIterator::~VHEIterator() { }



/// LIST
/*
inline HalfEdgeList VHEIterator::list() const {
    HalfEdgeList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}
*/

inline HalfEdgeList VHEIterator::list() const {
    HalfEdgeList L;
    HalfEdge_ptr h1 = m_v->HE();
    HalfEdge_ptr h2 = m_v->HE()->Prev()->Twin();
    do {
        if (h1 == h2) break;
        if (h1 != NULL)
        {
            L.push_back(h1);
            h1 = h1->Twin()->Next();
        }
        if (h2 != NULL)
        {
            L.push_back(h2);
            h2 = h2->Prev()->Twin();
        }
    } while( h1 != NULL || h2 != NULL );
    return L;
}



/// OPERATOR
inline HalfEdge* VHEIterator::operator->() const {
    return m_he.get();
}



} // namespace Core
} // namespace Ra
