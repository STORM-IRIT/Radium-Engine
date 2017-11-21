#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {



/// CLEAR
inline void Dcel::clear() {
    m_vertex.clear();
    m_halfedge.clear();
    m_fulledge.clear();
    m_face.clear();
}



/// QUERY
inline bool Dcel::empty() const {
    return ( m_vertex.empty()   &&
             m_halfedge.empty() &&
             m_fulledge.empty() &&
             m_face.empty() );
}


} // namespace Core
} // namespace Ra

