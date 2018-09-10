#include <Core/Mesh/DCEL/HalfEdge.hpp>

#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// VERTEX
inline const Vertex_ptr& HalfEdge::V() const {
    return m_v;
}

inline Vertex_ptr& HalfEdge::V() {
    return m_v;
}

inline void HalfEdge::setV( const Vertex_ptr& v ) {
    m_v = v;
}

/// NEXT
inline const HalfEdge_ptr& HalfEdge::Next() const {
    return m_next;
}

inline HalfEdge_ptr& HalfEdge::Next() {
    return m_next;
}

inline void HalfEdge::setNext( const HalfEdge_ptr& next ) {
    m_next = next;
}

/// PREV
inline const HalfEdge_ptr& HalfEdge::Prev() const {
    return m_prev;
}

inline HalfEdge_ptr& HalfEdge::Prev() {
    return m_prev;
}

inline void HalfEdge::setPrev( const HalfEdge_ptr& prev ) {
    m_prev = prev;
}

/// TWIN
inline const HalfEdge_ptr& HalfEdge::Twin() const {
    return m_twin;
}

inline HalfEdge_ptr& HalfEdge::Twin() {
    return m_twin;
}

inline void HalfEdge::setTwin( const HalfEdge_ptr& twin ) {
    CORE_ASSERT( twin->m_v->idx != m_v->idx, "Twins with same starting vertex." );
    m_twin = twin;
}

/// HALFEDGE
inline void HalfEdge::setNeighborHE( const HalfEdge_ptr& next, const HalfEdge_ptr& prev,
                                     const HalfEdge_ptr& twin ) {
    setNext( next );
    setPrev( prev );
    setTwin( twin );
}

/// FULLEDGE
inline const FullEdge_ptr& HalfEdge::FE() const {
    return m_fe;
}

inline FullEdge_ptr& HalfEdge::FE() {
    return m_fe;
}

inline void HalfEdge::setFE( const FullEdge_ptr& fe ) {
    m_fe = fe;
}

/// FACE
inline const Face_ptr& HalfEdge::F() const {
    return m_f;
}

inline Face_ptr& HalfEdge::F() {
    return m_f;
}

inline void HalfEdge::setF( const Face_ptr& f ) {
    m_f = f;
}

} // namespace Core
} // namespace Ra
