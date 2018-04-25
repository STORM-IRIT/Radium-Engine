#include <Core/Geometry/DCEL/Vertex.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// POINT
inline Math::Vector3 Vertex::P() const {
    return m_p;
}

inline Math::Vector3& Vertex::P() {
    return m_p;
}

inline void Vertex::setP( const Math::Vector3& p ) {
    m_p = p;
}

/// NORMAL
inline Math::Vector3 Vertex::N() const {
    return m_n;
}

inline Math::Vector3& Vertex::N() {
    return m_n;
}

inline void Vertex::setN( const Math::Vector3& n ) {
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
