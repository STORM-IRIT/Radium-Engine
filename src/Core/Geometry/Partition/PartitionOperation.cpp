#include <Core/Geometry/Partition/PartitionOperation.hpp>

#include <algorithm>
#include <limits>


namespace Ra {
namespace Core {
namespace Geometry {



VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Index& id, const bool is_max ) {
    VertexSegment v;
    for( uint i = 0; i < weights.size(); ++i ) {
        Scalar max = std::numeric_limits< Scalar >::min();
        uint j( -1 );
        for( const auto& w : weights[i] ) {
            if( max < w.second ) {
                max = w.second;
            }
            if( w.first == id ) {
                j = id;
            }
        }
        if( ( j != uint( -1 ) ) && ( ( weights[i][j].second == max ) || !is_max ) ) {
            v.insert( i );
        }
    }
    return v;
}



VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const IndexSet& index, const bool is_max ) {
    VertexSegment v;
    for( const auto& id : index ) {
        auto set = extractVertexSegment( weights, id, is_max );
        v.insert( set.begin(), set.end() );
    }
    return v;
}



VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Edge& edge, const bool is_max ) {
    VertexSegment v;
    auto set = extractVertexSegment( weights, edge( 0 ), is_max );
    v.insert( set.begin(), set.end() );
    set = extractVertexSegment( weights, edge( 1 ), is_max );
    v.insert( set.begin(), set.end() );
    return v;
}



VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const EdgeSet& edge, const bool is_max ) {
    VertexSegment v;
    for( const auto& e : edge ) {
        auto set = extractVertexSegment( weights, e, is_max );
        v.insert( set.begin(), set.end() );
    }
    return v;
}




VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Index& id, const bool is_max ) {
    VertexSegment v;
    Eigen::VectorXd col = weights.col( id );
    Scalar max = col.maxCoeff();
    for( uint i = 0; i < col.size(); ++i ) {
        if( ( col.coeff( i ) != 0.0 ) && ( ( col.coeff( i ) == max ) || !is_max ) ) {
            v.insert( i );
        }
    }
    return v;
}



VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const IndexSet& index, const bool is_max ) {
    VertexSegment v;
    for( const auto& id : index ) {
        auto set = extractVertexSegment( weights, id, is_max );
        v.insert( set.begin(), set.end() );
    }
    return v;
}



VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Edge& edge, const bool is_max ) {
    VertexSegment v;
    auto set = extractVertexSegment( weights, edge( 0 ), is_max );
    v.insert( set.begin(), set.end() );
    set = extractVertexSegment( weights, edge( 1 ), is_max );
    v.insert( set.begin(), set.end() );
    return v;
}



VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const EdgeSet& edge, const bool is_max ) {
    VertexSegment v;
    for( const auto& e : edge ) {
        auto set = extractVertexSegment( weights, e, is_max );
        v.insert( set.begin(), set.end() );
    }
    return v;
}



BitSet extractBitSet( const VertexSegment& v, const uint vertex_size ) {
    BitSet bit( vertex_size, false );
    for( const auto& id : v ) {
        bit[id] = true;
    }
    return bit;
}



TriangleSegment extractTriangleSegment( const BitSet& bit, const VectorArray< Triangle >& t ) {
    TriangleSegment T;
    for( uint i = 0; i < t.size(); ++i ) {
        if( bit[ t[i]( 0 ) ] && bit[ t[i]( 1 ) ] && bit[ t[i]( 2 ) ] ) {
            T.insert( i );
        }
    }
    return T;
}



}
}
}
