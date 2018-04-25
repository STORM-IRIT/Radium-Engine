#include <Core/Geometry/DCEL/Face.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
Face::Face( const Container::Index& index ) : IndexedObject( index ), m_he( nullptr ) {}

Face::Face( const HalfEdge_ptr& he, const Container::Index& index ) : IndexedObject( index ), m_he( he ) {}

/// DESTRUCTOR
Face::~Face() {}

} // namespace Core
} // namespace Ra
