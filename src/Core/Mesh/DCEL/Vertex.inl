#include <Core/Mesh/DCEL/Vertex.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// POINT
inline Vector3 Vertex::P() const {
    return m_p;
}

inline Vector3& Vertex::P() {
    return m_p;
}

inline void Vertex::setP( const Vector3& p ) {
    m_p = p;
}

/// NORMAL
inline Vector3 Vertex::N() const {
    return m_n;
}

inline Vector3& Vertex::N() {
    return m_n;
}

inline void Vertex::setN( const Vector3& n ) {
    m_n = n;
}

/// HALFEDGE
inline HalfEdge_ptr Vertex::HE() const {
    return m_he;
}

inline HalfEdge_ptr& Vertex::HE() {
    return m_he;
}

inline void Vertex::setHE( const HalfEdge_ptr& he ) {
    m_he = he;
}

} // namespace Core
} // namespace Ra
