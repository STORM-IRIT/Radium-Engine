#include "Edge.hpp"

#include "HalfEdge.hpp"

namespace Ra {
namespace Core {
namespace Dcel {

/// HALFEDGE
inline HalfEdge_ptr  Edge::HE() const {
    return m_he;
}

inline HalfEdge_ptr& Edge::HE() {
    return m_he;
}

inline void Edge::setHE( const HalfEdge_ptr& he ) {
    m_he = he;
}

/// OPERATOR
inline bool Edge::operator==( const Edge& e ) const {
    // Check if the ids are the same
    return ( ( ( m_he->idx == e.m_he->idx ) && ( m_he->Twin()->idx == e.m_he->Twin()->idx ) ) ||
             ( ( m_he->idx == e.m_he->Twin()->idx ) && ( m_he->Twin()->idx == e.m_he->idx ) ) );
}

inline bool Edge::operator<( const Edge& e ) const {
    Index min = std::min( m_he->V()->idx, m_he->Twin()->V()->idx );
    Index max = std::max( m_he->V()->idx, m_he->Twin()->V()->idx );
    Index e_min = std::min( e.m_he->V()->idx, e.m_he->Twin()->V()->idx );
    Index e_max = std::max( e.m_he->V()->idx, e.m_he->Twin()->V()->idx );
    if( min < e_min ) return true;
    if( min == e_min ) {
        if( max < e_max ) return true;
        return false;
    }
    return false;
}

} // namespace DCEL
} // namespace Core
} // namespace Ra
