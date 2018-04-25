#include <Core/Geometry/DCEL/Vertex.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
Vertex::Vertex( const Container::Index& index ) :
    IndexedObject( index ),
    m_p( Math::Vector3::Zero() ),
    m_n( Math::Vector3::Zero() ),
    m_he( nullptr ) {}

Vertex::Vertex( const HalfEdge_ptr& he ) :
    IndexedObject(),
    m_p( Math::Vector3::Zero() ),
    m_n( Math::Vector3::Zero() ),
    m_he( he ) {}

Vertex::Vertex( const Math::Vector3& p, const Math::Vector3& n, const HalfEdge_ptr& he, const Container::Index& index ) :
    IndexedObject( index ),
    m_p( p ),
    m_n( n ),
    m_he( he ) {}

/// DESTRUCTOR
Vertex::~Vertex() {}

} // namespace Core
} // namespace Ra
