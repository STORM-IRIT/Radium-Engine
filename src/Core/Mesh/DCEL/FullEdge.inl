#include <Core/Mesh/DCEL/FullEdge.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {



/// HALFEDGE
inline HalfEdge_ptr FullEdge::HE() const {
    return m_he;
}



inline HalfEdge_ptr& FullEdge::HE() {
    return m_he;
}



inline void FullEdge::setHE( const HalfEdge_ptr& he ) {
    m_he = he;
}



/// OPERATOR
inline bool FullEdge::operator==( const FullEdge& e ) const {
    CORE_ASSERT( ( m_he->Twin()   != nullptr ), "LHS twin is nullptr" );
    CORE_ASSERT( ( e.HE()->Twin() != nullptr ), "RHS twin is nullptr" );

    // Check if the ids are the same
    return ( ( ( m_he->idx == e.m_he->idx ) && ( m_he->Twin()->idx == e.m_he->Twin()->idx ) ) ||
             ( ( m_he->idx == e.m_he->Twin()->idx ) && ( m_he->Twin()->idx == e.m_he->idx ) ) );
}



inline bool FullEdge::operator<( const FullEdge& e ) const {
    CORE_ASSERT( ( m_he->Twin()   != nullptr ), "LHS twin is nullptr" );
    CORE_ASSERT( ( e.HE()->Twin() != nullptr ), "RHS twin is nullptr" );
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



} // namespace Core
} // namespace Ra
