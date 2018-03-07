#include <Core/Mesh/DCEL/HalfEdge.hpp>

#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
HalfEdge::HalfEdge( const Index& index ) :
    IndexedObject( index ),
    m_v( nullptr ),
    m_next( nullptr ),
    m_prev( nullptr ),
    m_twin( nullptr ),
    m_f( nullptr ) {}

HalfEdge::HalfEdge( const Vertex_ptr& v, const HalfEdge_ptr& next, const HalfEdge_ptr& prev,
                    const HalfEdge_ptr& twin, const Face_ptr& f, const Index& index ) :
    IndexedObject( index ),
    m_v( v ),
    m_next( next ),
    m_prev( prev ),
    m_twin( twin ),
    m_f( f ) {}

/// DESTRUCTOR
HalfEdge::~HalfEdge() {}

} // namespace Core
} // namespace Ra
