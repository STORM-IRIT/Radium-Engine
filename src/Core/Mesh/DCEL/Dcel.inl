#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {



/// CLEAR
inline void Dcel::clear() {
    m_vertex.clear();
    m_halfedge.clear();
    m_face.clear();
}



/// QUERY
inline bool Dcel::empty() const {
    return ( m_vertex.empty()   &&
             m_halfedge.empty() &&
             m_face.empty() );
}



inline bool Dcel::compact() const {
    return ( m_vertex.compact()   &&
             m_halfedge.compact() &&
             m_face.compact() );
}



} // namespace Core
} // namespace Ra

