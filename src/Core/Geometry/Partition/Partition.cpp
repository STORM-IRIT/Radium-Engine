#include <Core/Geometry/Partition/Partition.hpp>

#include <algorithm>
#include <limits>

namespace Ra {
namespace Core {
namespace Geometry {

VertexSegment extractVertexSegment( const Animation::MeshWeight& weights, const Index& id,
                                    const bool use_max ) {
    VertexSegment v;
    for ( uint i = 0; i < weights.size(); ++i )
    {
        Scalar max = std::numeric_limits<Scalar>::min();
        uint j( -1 );
        for ( const auto& w : weights[i] )
        {
            if ( max < w.second )
            {
                max = w.second;
            }
            if ( w.first == id )
            {
                j = id;
            }
        }
        if ( ( j != uint( -1 ) ) && ( ( weights[i][j].second == max ) || !use_max ) )
        {
            v.push_back( i );
        }
    }
    return v;
}

VertexSegment extractVertexSegment( const Animation::WeightMatrix& weights, const Index& id,
                                    const bool use_max ) {
    VertexSegment v;
    const VectorN col = weights.col( id );
    const Scalar max = col.maxCoeff();
    for ( int i = 0; i < col.size(); ++i )
    {
        if ( ( col.coeff( i ) != 0.0 ) && ( ( col.coeff( i ) == max ) || !use_max ) )
        {
            v.push_back( i );
        }
    }
    return v;
}

BitSet extractBitSet( const VertexSegment& v, const uint vertex_size ) {
    BitSet bit( vertex_size, false );
    for ( const auto& id : v )
    {
        bit[id] = true;
    }
    return bit;
}

TriangleSegment extractTriangleSegment( const BitSet& bit, const VectorArray<Triangle>& t ) {
    TriangleSegment T;
    for ( uint i = 0; i < t.size(); ++i )
    {
        // Ternary XOR. A triangle belong to the partition if one or all the vertices are in the
        // partition.
        if ( !( !bit[t[i]( 0 )] != !bit[t[i]( 1 )] ) != ( !bit[t[i]( 2 )] ) )
        {
            T.push_back( i );
        }
    }
    return T;
}

MeshPartition partition( const TriangleMesh& mesh, const Animation::WeightMatrix& weight,
                         const bool use_max ) {
    const uint size = weight.cols();
    const uint v_size = mesh.vertices().size();
    MeshPartition part( size );
#pragma omp parallel for
    for ( uint n = 0; n < size; ++n )
    {
        const VertexSegment v = extractVertexSegment( weight, n, use_max );
        const BitSet b = extractBitSet( v, v_size );
        const TriangleSegment t = extractTriangleSegment( b, mesh.m_triangles );
        part[n].vertices().resize( v.size() );
        part[n].normals().resize( v.size() );
        part[n].m_triangles.resize( t.size() );
        std::map<uint, uint> id;
        for ( uint i = 0; i < v.size(); ++i )
        {
            id[v[i]] = i;
            part[n].vertices()[i] = mesh.vertices()[v[i]];
            part[n].normals()[i] = mesh.normals()[v[i]];
        }
        for ( uint i = 0; i < t.size(); ++i )
        {
            const Triangle& T = mesh.m_triangles[t[i]];
            part[n].m_triangles[i] = Triangle( id[T[0]], id[T[1]], id[T[2]] );
        }
    }
    return part;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
