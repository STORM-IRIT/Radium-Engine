#include "Face.hpp"

#include "HalfEdge.hpp"

namespace Ra {
namespace Core {
namespace Dcel {

/// HALFEDGE
inline HalfEdge_ptr  Face::HE() const {
    return m_he;
}

inline HalfEdge_ptr& Face::HE() {
    return m_he;
}

inline void Face::setHE( const HalfEdge_ptr& he ) {
    m_he = he;
}

} // namespace DCEL
} // namespace Core
} // namespace Ra

