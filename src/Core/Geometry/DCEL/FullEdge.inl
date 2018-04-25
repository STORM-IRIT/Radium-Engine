#include <Core/Geometry/DCEL/FullEdge.hpp>

#include <Core/Geometry/DCEL/Face.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>
#include <Core/Geometry/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

/// VERTEX
inline Vertex_ptr FullEdge::V( const uint i ) const {
    return HE( i )->V();
}

/// HALFEDGE
inline HalfEdge_ptr FullEdge::HE( const uint i ) const {
    CORE_ASSERT( ( i < 2 ), "Index i out of bound" );
    if ( i == 0 )
    {
        return m_he;
    }
    return m_he->Twin();
}

inline HalfEdge_ptr& FullEdge::HE( const uint i ) {
    CORE_ASSERT( ( i < 2 ), "Index i out of bound" );
    if ( i == 0 )
    {
        return m_he;
    }
    return m_he->Twin();
}

inline void FullEdge::setHE( const HalfEdge_ptr& he ) {
    m_he = he;
}

/// FACE
inline Face_ptr FullEdge::F( const uint i ) const {
    return HE( i )->F();
}

/// FULLEDGE
inline FullEdge_ptr FullEdge::Head( const uint i ) const {
    CORE_ASSERT( ( i < 2 ), "Index i out of bound" );
    if ( i == 0 )
    {
        return HE( 0 )->Next()->FE();
    }
    return HE( 1 )->Prev()->FE();
}

inline FullEdge_ptr FullEdge::Tail( const uint i ) const {
    CORE_ASSERT( ( i < 2 ), "Index i out of bound" );
    if ( i == 0 )
    {
        return HE( 0 )->Prev()->FE();
    }
    return HE( 1 )->Next()->FE();
}

/// OPERATOR
inline bool FullEdge::operator==( const FullEdge& e ) const {
    CORE_ASSERT( ( m_he->Twin() != nullptr ), "LHS twin is nullptr" );
    CORE_ASSERT( ( e.HE( 0 )->Twin() != nullptr ), "RHS twin is nullptr" );

    // Check if the ids are the same
    return ( ( ( m_he->idx == e.m_he->idx ) && ( m_he->Twin()->idx == e.m_he->Twin()->idx ) ) ||
             ( ( m_he->idx == e.m_he->Twin()->idx ) && ( m_he->Twin()->idx == e.m_he->idx ) ) );
}

inline bool FullEdge::operator<( const FullEdge& e ) const {
    CORE_ASSERT( ( m_he->Twin() != nullptr ), "LHS twin is nullptr" );
    CORE_ASSERT( ( e.HE( 0 )->Twin() != nullptr ), "RHS twin is nullptr" );
    Container::Index min = std::min( m_he->V()->idx, m_he->Twin()->V()->idx );
    Container::Index max = std::max( m_he->V()->idx, m_he->Twin()->V()->idx );
    Container::Index e_min = std::min( e.m_he->V()->idx, e.m_he->Twin()->V()->idx );
    Container::Index e_max = std::max( e.m_he->V()->idx, e.m_he->Twin()->V()->idx );
    if ( min < e_min )
        return true;
    if ( min == e_min )
    {
        if ( max < e_max )
            return true;
        return false;
    }
    return false;
}

} // namespace Core
} // namespace Ra
