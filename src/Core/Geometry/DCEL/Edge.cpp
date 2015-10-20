#include <Core/Geometry/DCEL/Edge.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/// CONSTRUCTOR
Edge::Edge() : IndexedObject (), m_he( nullptr ) { }

Edge::Edge( const Index& index ) : IndexedObject ( index ), m_he( nullptr ) { }

Edge::Edge( const HalfEdge_ptr& he ) : IndexedObject (), m_he( he ) { }

Edge::Edge( const Index&        index,
            const HalfEdge_ptr& he     ) : IndexedObject ( index ), m_he( he ) { }

Edge::Edge( const Edge& edge ) : IndexedObject ( edge ), m_he( edge.m_he ) { }

/// DESTRUCTOR
Edge::~Edge() { }

} // namespace DCEL
} // namespace Core
} // namespace Ra
