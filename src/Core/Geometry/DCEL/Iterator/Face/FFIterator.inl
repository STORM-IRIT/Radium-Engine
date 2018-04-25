#include <Core/Geometry/DCEL/Iterator/Face/FFIterator.hpp>

#include <Core/Geometry/DCEL/Face.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
FFIterator::FFIterator( Face_ptr& f ) : FIterator<Face>( f ) {}

/// DESTRUCTOR
FFIterator::~FFIterator() {}

/// LIST
inline FaceList FFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_f->HE();
    do
    {
        L.push_back( it->F() );
        it = it->Next();
    } while ( it != m_f->HE() );
    return L;
}

/// OPERATOR
inline Face* FFIterator::operator->() const {
    return m_he->Twin()->F().get();
}

} // namespace Core
} // namespace Ra
