#include <Core/Geometry/DCEL/Iterator/Vertex/VVIterator.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>
#include <Core/Geometry/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VVIterator::VVIterator( Vertex_ptr& v ) : VIterator<Vertex>( v ) {}

/// DESTRUCTOR
VVIterator::~VVIterator() {}

/// LIST
inline VertexList VVIterator::list() const {
    VertexList L;
    HalfEdge_ptr it = m_v->HE();
    do
    {
        L.push_back( it->Next()->V() );
        it = it->Prev()->Twin();
    } while ( it != m_v->HE() );
    return L;
}

/// OPERATOR
inline Vertex* VVIterator::operator->() const {
    return m_he->Next()->V().get();
}

} // namespace Core
} // namespace Ra
