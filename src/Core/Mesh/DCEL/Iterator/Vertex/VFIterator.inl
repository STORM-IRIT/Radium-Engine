#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



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
