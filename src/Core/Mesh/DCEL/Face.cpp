#include <Core/Mesh/DCEL/Face.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
Face::Face( const Index& index ) : IndexedObject( index ), m_he( nullptr ) {}

Face::Face( const HalfEdge_ptr& he, const Index& index ) : IndexedObject( index ), m_he( he ) {}

/// DESTRUCTOR
Face::~Face() {}

} // namespace Core
} // namespace Ra
