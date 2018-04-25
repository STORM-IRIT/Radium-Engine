#include <Core/Geometry/DCEL/Iterator/Face/FaceIterator.hpp>

#include <Core/Geometry/DCEL/Face.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>
#include <Core/Geometry/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
FVIterator::FVIterator( Face_ptr& f ) : FIterator<Vertex>( f ) {}

/// DESTRUCTOR
FVIterator::~FVIterator() {}

/// LIST
inline VertexList FVIterator::list() const {
    VertexList L;
    HalfEdge_ptr it = m_f->HE();
    do
    {
        L.push_back( it->V() );
        it = it->Next();
    } while ( it != m_f->HE() );
    return L;
}

/// OPERATOR
inline Vertex* FVIterator::operator->() const {
    return m_he->V().get();
}

} // namespace Core
} // namespace Ra
