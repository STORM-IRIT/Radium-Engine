#include <Core/Geometry/DCEL/Face.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// HALFEDGE
inline HalfEdge_ptr Face::HE() const {
    return m_he;
}

inline HalfEdge_ptr& Face::HE() {
    return m_he;
}

inline void Face::setHE( const HalfEdge_ptr& he ) {
    m_he = he;
}

} // namespace Core
} // namespace Ra
