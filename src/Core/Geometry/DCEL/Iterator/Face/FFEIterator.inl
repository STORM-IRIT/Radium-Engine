#include <Core/Geometry/DCEL/Iterator/Face/FFEIterator.hpp>

#include <Core/Geometry/DCEL/Face.hpp>
#include <Core/Geometry/DCEL/FullEdge.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
FFEIterator::FFEIterator( Face_ptr& f ) : FIterator<FullEdge>( f ) {}

/// DESTRUCTOR
FFEIterator::~FFEIterator() {}

/// LIST
inline FullEdgeList FFEIterator::list() const {
    FullEdgeList L;
    HalfEdge_ptr it = m_f->HE();
    do
    {
        L.push_back( it->FE() );
        it = it->Next();
    } while ( it != m_f->HE() );
    return L;
}

/// OPERATOR
inline FullEdge* FFEIterator::operator->() const {
    return m_he->FE().get();
}

} // namespace Core
} // namespace Ra
