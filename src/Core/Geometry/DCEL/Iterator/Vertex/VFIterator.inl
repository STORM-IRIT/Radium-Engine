#include <Core/Geometry/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Geometry/DCEL/Face.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>
#include <Core/Geometry/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VFIterator::VFIterator( Vertex_ptr& v ) : VIterator<Face>( v ) {}

/// DESTRUCTOR
VFIterator::~VFIterator() {}

/// LIST
inline FaceList VFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_v->HE();
    do
    {
        L.push_back( it->F() );
        it = it->Prev()->Twin();
    } while ( it != m_v->HE() );
    return L;
}

/// OPERATOR
inline Face* VFIterator::operator->() const {
    return m_he->F().get();
}

} // namespace Core
} // namespace Ra
