#include <Core/Utils/Graph/AdjacencyListOperation.hpp>

#include <utility>
#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Graph {


void check( const AdjacencyList& adj ) {
#if defined CORE_DEBUG
    CORE_ASSERT( ( adj.m_parent.size() == adj.m_child.size() ), "Children list and parent list are incompatible.");
    // Check no node is there before its parent.
    for( uint node = 0; node < adj.size(); ++node ) {
        CORE_ASSERT( ( adj.m_parent.at( node ) < int( node ) ), "Parent ordering is wrong.");
        CORE_ASSERT( ( adj.m_parent.at( node ) >= -1 ), "Parent index is wrong");
        for( const auto& child : adj.m_child.at( node ) ) {
            CORE_ASSERT( ( adj.m_parent.at( child ) == node ), " Inconsistent parent indexes");
        }
        CORE_ASSERT( adj.isLeaf( node ) == ( adj.m_child.at( node ).size() == 0 ), " Only leaves should have no children.");
    }
#endif
}



AdjacencyList extractAdjacencyList( const VectorArray< Edge >& edgeList ) {
    AdjacencyList adj;
    for( const auto& edge : edgeList ) {
        uint size =  std::max(  uint(adj.m_child.size() - 1 ), std::max( edge( 0 ), edge( 1 ) ) );
        adj.m_child.resize( size + 1 );
        adj.m_child[ edge( 0 ) ].push_back( edge( 1 ) );
        adj.m_parent.resize( size, -1 );
        adj.m_parent[ edge( 1 ) ] = edge( 0 );
    }

    // TODO: reorganize the adjacent list so check( adj ) won't fail.

    return adj;
}



VectorArray< Edge > extractEdgeList( const AdjacencyList& adj, const bool include_leaf ) {
    VectorArray< Edge > edgeList;
    for( uint i = 0; i < adj.m_child.size(); ++i ) {
        if( include_leaf && adj.isLeaf( i ) ) {
            Edge e;
            e( 0 ) = i;
            e( 1 ) = i;
            edgeList.push_back( e );
        } else {
            for( const auto& edge : adj.m_child[i] ) {
                Edge e;
                e( 0 ) = i;
                e( 1 ) = edge;
                edgeList.push_back( e );
            }
        }
    }
    return edgeList;
}

} // namespace GraphicsEntity
} // namespace Core
} // namespace Ra
