#include <Core/Geometry/DCEL/FullEdge.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
FullEdge::FullEdge( const Container::Index& index ) : IndexedObject( index ), m_he( nullptr ) {}

FullEdge::FullEdge( const HalfEdge_ptr& he, const Container::Index& index ) :
    IndexedObject( index ),
    m_he( he ) {}

/// DESTRUCTOR
FullEdge::~FullEdge() {}

} // namespace Core
} // namespace Ra
