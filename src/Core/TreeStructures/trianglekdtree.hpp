#ifndef TRIANGLEKDTREE_H
#define TRIANGLEKDTREE_H

#include <Eigen/Core>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>
#include <Core/Geometry/Distance/DistanceQueries.hpp>

#include <iostream>
#include <limits>
#include <numeric>

// max depth of the tree
#define KD_MAX_DEPTH 32

// number of neighbors
#define KD_TRIANGLES_PER_CELL 64

namespace Ra {
namespace Core {

/**
 * A TriangleKdTree is a space partitionning structure storing 3D Triangles.
 * Here, the TriangleKdTree is built to be a balanced binary tree.
 * Each inner node stores the splitting plane position and orientation.
 * Each leave node stores triangles inside (and intersecting) the space it represents.
 *
 * \tparam _Index the type used to index triangles in the tree.
 */
// TODO: Ask Anahid for a ref about the implementation.
template <typename _Index = int>
class TriangleKdTree {
  public:
    RA_CORE_ALIGNED_NEW

    /// The type for indices.
    using Index = _Index;
    /// The type for indices lists.
    using IndexList = std::vector<Index>;

    /// The type for 3D points.
    using VectorType = Ra::Core::Vector3;

    /// The type for 3D Triangles.
    using Triangle = Ra::Core::Triangle;

    /// The type for 3D Axis-Aligned Bounding Boxes.
    using AxisAlignedBoxType = Ra::Core::Aabb;

    /// The type for 3D points Lists.
    using PointList = Ra::Core::VectorArray<VectorType>;

    /// The type for 3D Triangles Lists.
    using TriangleList = Ra::Core::VectorArray<Triangle>;

    /**
     * Build a KdTree from the given lists of Triangles and corresponding 3D points.
     * \param t the list of triangles.
     * \param p the list of points.
     * \param nofTrianglesPerCell the maximal allowed number of triangles per tree node.
     * \param maxDepth the maximal allowed depth for the tree.
     * \note If \p maxDepth is reached, leaf nodes can have more than \p nofTrianglesPerCell triangles.
     */
    TriangleKdTree( const TriangleList& t = TriangleList(),
                    const PointList& p = PointList(),
                    uint nofTrianglesPerCell = KD_TRIANGLES_PER_CELL,
                    uint maxDepth = KD_MAX_DEPTH );

    ~TriangleKdTree();

    /// Add the given triangles with their corresponding points to the tree.
    void insertTriangles( const TriangleList& t, const PointList& p );

    /// Rebuild the tree if its content changed.
    inline void update();

    /// \returns an invalid Index.
    static constexpr Index invalidIndex() { return -1; }

    /// \name Data access
    /// @{
    /// \returns the list of 3D points.
    inline const PointList& getPoints( void ) const { return m_points; }

    /// \returns the list of 3D Triangles.
    inline const TriangleList& getTriangles( void ) const { return m_triangles; }

    /// \returns the Axis-Aligned Bounding Box of the Triangles.
    inline const AxisAlignedBoxType& getAabb() const { return m_aabb; }
    /// @}

    /// \name Tree Queries
    /// @{
    /// \returns the index of the Triangle closest to \p p if closer than
    ///          sqrt( \p sqDist ), an invalid index otherwise.
    inline Index doQueryRestrictedClosestIndex( const VectorType& p, Scalar sqDist ) const;

    /// \returns the index of the Triangle closest to the segment [\p s1,\p s2]
    ///          if closer than sqrt( \p sqDist ), an invalid index otherwise.
    inline Index doQueryRestrictedClosestIndex( const VectorType& s1, const VectorType& s2,
                                                Scalar sqDist ) const;

    /// Returns in \p cl_id the list of indices for Triangles closer to the segment [\p s1,\p s2]
    ///         than sqrt( \p sqDist ).
    inline void doQueryRestrictedClosestIndexes( const VectorType& s1, const VectorType& s2,
                                                 Scalar sqDist, IndexList& cl_id ) const;

    /// Returns in the index of the Triangles closest to the triangle [\p a,\p b,\p c]
    ///         if closer than sqrt( \p sqDist ), an invalid index otherwise.
    inline Index doQueryRestrictedClosestIndexTriangle( const VectorType& a, const VectorType& b,
                                                        const VectorType& c ) const;
    /// @}

  private:
    /**
     * A KdNode is a node in the KdTree, storing informations about the
     * splitting plane or the list of triangles it stores.
     */
    struct KdNode {
        IndexList triangleIndices; ///< list of triangles.
        float splitValue;          ///< split value along the dim coordinate.
        uint firstChildId : 24;    ///< index of the first child node.
        uint dim : 2;              ///< axis of the split.
        uint leaf : 1;             ///< whether the node is a leaf or not.
    };

    /// The type for KdNode Lists.
    using NodeList = std::vector<KdNode>;

    /// Node of the query stack.
    struct QueryNode {
        inline QueryNode() {}
        inline QueryNode( uint id ) : nodeId( id ) {}
        uint nodeId; ///< id of the next node.
        Scalar sq;           ///< squared distance to the next node.
    };

    /// Internal call to initialize the KdTree.
    inline void initialize();

    /// Internal call to split the space represented by node \p nodeId.
    /// \note The node is split so that triangles are evenly spread over children.
    inline void split( uint nodeId );

    /// Internal call to recursively split all nodes.
    void createTree( uint nodeId, float splitValue, uint level );

  private:
    NodeList m_nodes;          ///< The list of Nodes.
    PointList m_points;        ///< The list of Points.
    TriangleList m_triangles;  ///< The list of Triangles.
    IndexList m_indices;       ///< The list of triangles indices.
    AxisAlignedBoxType m_aabb; ///< The Axis-Aligned bounding box of the Triangles.
    bool m_upToDate;           ///< Whether the tree is up-to-date or not w.r.t. triangles.

    ///< The maximal allowed number of triangles per node.
    uint m_nofTrianglesPerCell;

    ///< The maximal allowed tree depth.
    uint m_maxDepth;
};

template <typename Index>
TriangleKdTree<Index>::TriangleKdTree( const TriangleList& t, const PointList& p,
                                       uint nofTrianglesPerCell, uint maxDepth ) :
    m_points( p ),
    m_triangles( t ),
    m_indices( m_triangles.size() ),
    m_nofTrianglesPerCell( nofTrianglesPerCell ),
    m_maxDepth( maxDepth ) {
    initialize();
}

template <typename Index>
TriangleKdTree<Index>::~TriangleKdTree() {}

template <typename Index>
void TriangleKdTree<Index>::insertTriangles( const TriangleList& t, const PointList& p ) {
    const uint n = m_points.size();
    m_points.insert( m_points.end(), p.begin(), p.end() );
    std::transform( t.begin(), t.end(), m_triangles.end(),
                    [n]( const Triangle& t ) { return Triangle( t(0) + n, t(1) + n, t( 2 ) + n ); } );
}

template <typename Index>
void TriangleKdTree<Index>::update() {
    if ( m_upToDate )
        return;
    initialize();
}

template <typename Index>
void TriangleKdTree<Index>::initialize() {
    // build the aabb
    for ( const auto& pt : m_points )
        m_aabb.extend( pt );
    // initialize the indices
    std::iota( m_indices.begin(), m_indices.end(), 0 );
    // prepare the node list
    m_nodes.reserve( 16 * m_triangles.size() / m_nofTrianglesPerCell );
    // tried : std::pow( 2, std::ceil( std::log2( m_triangles.size() / m_nofTrianglesPerCell ) ) )
    // but crashes... has to be investigated
    m_nodes.push_back( KdNode() );
    m_nodes.back().leaf = 0;
    m_nodes.back().triangleIndices = m_indices;
    // create the tree
    createTree( 0, 0, 1 );
    m_nodes.shrink_to_fit();
}

template <typename Index>
Index TriangleKdTree<Index>::doQueryRestrictedClosestIndex( const VectorType& p, Scalar sqDist ) const {
    Index cl_id = invalidIndex();
    Scalar cl_dist = sqDist;

    QueryNode nodeStack[2*KD_MAX_DEPTH];
    nodeStack[0].nodeId = 0;
    nodeStack[0].sq = 0.f;
    uint count = 1;

    while ( count )
    {
        QueryNode& qnode = nodeStack[count - 1];
        const KdNode& node = m_nodes[qnode.nodeId];

        if ( qnode.sq < cl_dist )
        {
            if ( node.leaf )
            {
                --count; // pop
                const uint nbTriangles = node.triangleIndices.size();
                for ( uint i = 0; i < nbTriangles; i++ )
                {
                    const Triangle& T = m_triangles[node.triangleIndices[i]];
                    const Scalar sqdist = Ra::Core::DistanceQueries::pointToTriSq(
                        p, m_points[T[0]], m_points[T[1]], m_points[T[2]] ).distanceSquared;
                    if ( sqdist < cl_dist )
                    {
                        cl_dist = sqdist;
                        cl_id = node.triangleIndices[i];
                    }
                }
            } else
            {
                const Scalar d = p[node.dim] - node.splitValue;
                if ( d < 0. )
                {
                    // the point is on the left side of the split plane
                    nodeStack[count].nodeId = node.firstChildId;
                    qnode.nodeId = node.firstChildId + 1;
                } else
                {
                    // the point is on the right side of the split plane
                    nodeStack[count].nodeId = node.firstChildId + 1;
                    qnode.nodeId = node.firstChildId;
                }
                nodeStack[count].sq = qnode.sq;
                qnode.sq = d * d;
                ++count;
            }
        } else
        {
            --count;
        }
    }
    return cl_id;
}

template <typename Index>
Index TriangleKdTree<Index>::doQueryRestrictedClosestIndex( const VectorType& s1,
                                                            const VectorType& s2, Scalar sqDist ) const {
    Index cl_id = invalidIndex();
    Scalar cl_dist = sqDist;

    QueryNode nodeStack[2*KD_MAX_DEPTH];
    nodeStack[0].nodeId = 0;
    nodeStack[0].sq = 0.f;
    uint count = 1;

    const VectorType segCenter = Scalar(0.5) * ( s1 + s2 );
    const VectorType segDirection = s2 - s1;
    const Scalar segExtent = Scalar(0.5) * segDirection.norm();

    while ( count )
    {
        QueryNode& qnode = nodeStack[count - 1];
        const KdNode& node = m_nodes[qnode.nodeId];

        if ( qnode.sq < cl_dist )
        {
            if ( node.leaf )
            {
                --count;
                const uint nbTriangles = node.triangleIndices.size();
                for ( uint i = 0; i < nbTriangles; i++ )
                {
                    const Triangle& T = m_triangles[node.triangleIndices[i]];
                    const VectorType triangle[3] = {m_points[T[0]], m_points[T[1]], m_points[T[2]]};
                    const Scalar sqdist = Ra::Core::DistanceQueries::segmentToTriSq(
                                              segCenter, segDirection, segExtent, triangle )
                                              .sqrDistance;
                    if ( sqdist < cl_dist )
                    {
                        cl_dist = sqdist;
                        cl_id = node.triangleIndices[i];
                    }
                }
            } else
            {
                Scalar new_off;
                const Scalar d1 = s1[node.dim] - node.splitValue;
                const Scalar d2 = s2[node.dim] - node.splitValue;
                if ( d1 < 0. && d2 < 0. )
                {
                    // the segment is on the left side of the split plane
                    new_off = std::max( d1, d2 );
                    nodeStack[count].nodeId = node.firstChildId;
                    qnode.nodeId = node.firstChildId + 1;
                } else if ( d1 >= 0. && d2 >= 0. )
                {
                    // the segment is on the right side of the split plane
                    new_off = std::min( d1, d2 );
                    nodeStack[count].nodeId = node.firstChildId + 1;
                    qnode.nodeId = node.firstChildId;
                } else
                {
                    // the segment is intersecting the split plane
                    new_off = 0;
                    const VectorType& s = ( s1 + s2 ) / 2;
                    const Scalar d = s[node.dim] - node.splitValue;
                    if ( d < 0. )
                    {
                        nodeStack[count].nodeId = node.firstChildId;
                        qnode.nodeId = node.firstChildId + 1;
                    } else
                    {
                        nodeStack[count].nodeId = node.firstChildId + 1;
                        qnode.nodeId = node.firstChildId;
                    }
                }
                nodeStack[count].sq = qnode.sq;
                qnode.sq = new_off * new_off;
                ++count;
            }
        } else
        {
            --count;
        }
    }
    return cl_id;
}

template <typename Index>
void TriangleKdTree<Index>::doQueryRestrictedClosestIndexes(const VectorType& s1,
                                                             const VectorType& s2, Scalar sqDist,
                                                             IndexList &cl_id ) const {
    QueryNode mNodeStack[2*KD_MAX_DEPTH];
    mNodeStack[0].nodeId = 0;
    mNodeStack[0].sq = 0.f;
    uint count = 1;

    const VectorType segCenter = Scalar( 0.5 ) * ( s1 + s2 );
    const VectorType segDirection = s2 - s1;
    const Scalar segExtent = Scalar(0.5) * segDirection.norm();

    while ( count )
    {
        QueryNode& qnode = mNodeStack[count - 1];
        const KdNode& node = m_nodes[qnode.nodeId];

        if ( qnode.sq < sqDist )
        {
            if ( node.leaf )
            {
                --count;
                const uint nbTriangles = node.triangleIndices.size();
                for ( uint i = 0; i < nbTriangles; i++ )
                {
                    const Triangle& T = m_triangles[node.triangleIndices[i]];
                    const VectorType triangle[3] = {m_points[T[0]], m_points[T[1]], m_points[T[2]]};
                    const Scalar dist = Ra::Core::DistanceQueries::segmentToTriSq(
                                            segCenter, segDirection, segExtent, triangle )
                                            .sqrDistance;
                    if ( dist < sqDist )
                    {
                        cl_id.push_back( node.triangleIndices[i] );
                    }
                }
            } else
            {
                Scalar new_off;
                const Scalar d1 = s1[node.dim] - node.splitValue;
                const Scalar d2 = s2[node.dim] - node.splitValue;
                if ( d1 < 0. && d2 < 0. )
                {
                    // the segment is on the left side of the split plane
                    new_off = std::max( d1, d2 );
                    mNodeStack[count].nodeId = node.firstChildId;
                    qnode.nodeId = node.firstChildId + 1;
                } else if ( d1 >= 0. && d2 >= 0. )
                {
                    // the segment is on the right side of the split plane
                    new_off = std::min( d1, d2 );
                    mNodeStack[count].nodeId = node.firstChildId + 1;
                    qnode.nodeId = node.firstChildId;
                } else
                {
                    // the segment is intersecting the split plane
                    new_off = 0;
                    const VectorType& s = ( s1 + s2 ) / 2;
                    const Scalar d = s[node.dim] - node.splitValue;
                    if ( d < 0. )
                    {
                        mNodeStack[count].nodeId = node.firstChildId;
                        qnode.nodeId = node.firstChildId + 1;
                    } else
                    {
                        mNodeStack[count].nodeId = node.firstChildId + 1;
                        qnode.nodeId = node.firstChildId;
                    }
                }
                mNodeStack[count].sq = qnode.sq;
                qnode.sq = new_off * new_off;
                ++count;
            }
        } else
        {
            --count;
        }
    }
}

template <typename Index>
Index TriangleKdTree<Index>::doQueryRestrictedClosestIndexTriangle( const VectorType& a,
                                                                    const VectorType& b,
                                                                    const VectorType& c ) const {
    Index cl_id = invalidIndex();
    Scalar cl_dist = std::numeric_limits<Scalar>::max();

    QueryNode mNodeStack[2*KD_MAX_DEPTH];
    mNodeStack[0].nodeId = 0;
    mNodeStack[0].sq = 0.f;
    uint count = 1;

    const VectorType triangle[3] = {a, b, c};

    while ( count )
    {
        QueryNode& qnode = mNodeStack[count - 1];
        const KdNode& node = m_nodes[qnode.nodeId];

        if ( qnode.sq < cl_dist )
        {
            if ( node.leaf )
            {
                --count;
                const uint nbTriangles = node.triangleIndices.size();
                for ( uint i = 0; i < nbTriangles; i++ )
                {
                    const Triangle& T = m_triangles[node.triangleIndices[i]];
                    const VectorType tri[3] = {m_points[T[0]], m_points[T[1]], m_points[T[2]]};
                    const Scalar sqdist =
                        Ra::Core::DistanceQueries::triangleToTriSq( triangle, tri ).sqrDistance;
                    if ( sqdist < cl_dist )
                    {
                        cl_dist = sqdist;
                        cl_id = node.triangleIndices[i];
                    }
                }
            } else
            {
                Scalar new_off;
                const Scalar d1 = a[node.dim] - node.splitValue;
                const Scalar d2 = b[node.dim] - node.splitValue;
                const Scalar d3 = c[node.dim] - node.splitValue;
                if ( d1 < 0. && d2 < 0. && d3 < 0. )
                {
                    // the triangle is on the left side of the split plane
                    new_off = std::max( std::max( d1, d2 ), d3 );
                    mNodeStack[count].nodeId = node.firstChildId;
                    qnode.nodeId = node.firstChildId + 1;
                } else if ( d1 >= 0. && d2 >= 0. && d3 >= 0. )
                {
                    // the triangle is on the right side of the split plane
                    new_off = std::min( std::min( d1, d2 ), d3 );
                    mNodeStack[count].nodeId = node.firstChildId + 1;
                    qnode.nodeId = node.firstChildId;
                } else
                {
                    // the segment is intersecting the split plane
                    new_off = 0;
                    const VectorType v = Ra::Core::Geometry::triangleBarycenter( a, b, c );
                    const Scalar d = v[node.dim] - node.splitValue;
                    if ( d < 0. )
                    {
                        mNodeStack[count].nodeId = node.firstChildId;
                        qnode.nodeId = node.firstChildId + 1;
                    } else
                    {
                        mNodeStack[count].nodeId = node.firstChildId + 1;
                        qnode.nodeId = node.firstChildId;
                    }
                }
                mNodeStack[count].sq = qnode.sq;
                qnode.sq = new_off * new_off;
                ++count;
            }
        } else
        {
            --count;
        }
    }
    return cl_id;
}

template <typename Index>
void TriangleKdTree<Index>::split( uint nodeId )
{
    KdNode& node = m_nodes[nodeId];

    auto& T0 = m_nodes[node.firstChildId].triangleIndices;
    auto& T1 = m_nodes[node.firstChildId+1].triangleIndices;
    for ( uint i = 0; i < node.triangleIndices.size(); i++ )
    {
        const Index& idx = node.triangleIndices[i];
        const Triangle& T = m_triangles[ idx ];
        char status = 0;
        if ( m_points[T[0]][node.dim] < node.splitValue )
            status |= 1;
        if ( m_points[T[1]][node.dim] < node.splitValue )
            status |= 2;
        if ( m_points[T[2]][node.dim] < node.splitValue )
            status |= 4;
        if ( status != 0 )
        {
            if ( status == 7 )
            {
                T0.push_back( idx );
            } else
            {
                T0.push_back( idx );
                T1.push_back( idx );
            }
        } else
        { T1.push_back( idx ); }
    }
    node.triangleIndices.clear();
}

template <typename Index>
void TriangleKdTree<Index>::createTree( uint nodeId, float splitValue, uint level ) {
    KdNode& node = m_nodes[nodeId];

    // compute the aabb
    AxisAlignedBoxType aabb = AxisAlignedBoxType();
    std::for_each( node.triangleIndices.begin(),
                   node.triangleIndices.end(),
                   [&aabb, this]( const Index& t ) {
                       const Triangle& T = this->m_triangles[ t ];
                       aabb.extend( this->m_points[T[0]] );
                       aabb.extend( this->m_points[T[1]] );
                       aabb.extend( this->m_points[T[2]] );
                   } );

    // get the split axis and value
    VectorType diag = Scalar( 0.5 ) * ( aabb.max() - aabb.min() );
    typename VectorType::Index dim;
    diag.maxCoeff( &dim );
    node.dim = dim;
    node.splitValue = aabb.center()( dim );

    // split
    if ( node.splitValue == splitValue && nodeId != 0 )
    {
        node.leaf = 1;
    } else
    {
        node.firstChildId = m_nodes.size();
        {
            KdNode n;
            m_nodes.push_back( n );
            m_nodes.push_back( n );
        }

        // sorting the triangles in regard to the split value of the selected axis (left and right)
        split( nodeId );

        // check for no split
        int matches = 0;
        const auto& T0 = m_nodes[node.firstChildId].triangleIndices;
        const auto& T1 = m_nodes[node.firstChildId+1].triangleIndices;
        for ( uint i = 0; i < T0.size(); i++ )
        {
            for ( uint j = 0; j < T1.size(); j++ )
            {
                if ( T0[i] == T1[j] )
                    matches++;
            }
        }

        if ( T0.size() == matches && T1.size() == matches )
        {
            m_nodes[node.firstChildId].leaf = 1;
            m_nodes[node.firstChildId + 1].leaf = 1;
        } else
        {
            {
                // left child
                uint childId = node.firstChildId;
                KdNode& child = m_nodes[childId];
                if ( child.triangleIndices.size() <= m_nofTrianglesPerCell ||
                     level >= m_maxDepth )
                {
                    child.leaf = 1;
                } else
                {
                    child.leaf = 0;
                    createTree( childId, node.splitValue, level + 1 );
                }
            }
            {
                // right child
                uint childId = node.firstChildId + 1;
                KdNode& child = m_nodes[childId];
                if ( child.triangleIndices.size() <= m_nofTrianglesPerCell ||
                     level >= m_maxDepth )
                {
                    child.leaf = 1;
                } else
                {
                    child.leaf = 0;
                    createTree( childId, node.splitValue, level + 1 );
                }
            }
        }
    }
}

} // namespace Core
} // namespace Ra

#endif // TRIANGLEKDTREE_H
