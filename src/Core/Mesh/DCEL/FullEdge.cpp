#include <Core/Mesh/DCEL/FullEdge.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
FullEdge::FullEdge( const Index& index ) : IndexedObject( index ), m_he( nullptr ) {}

FullEdge::FullEdge( const HalfEdge_ptr& he, const Index& index ) :
    IndexedObject( index ),
    m_he( he ) {}

/// DESTRUCTOR
FullEdge::~FullEdge() {}

} // namespace Core
} // namespace Ra
