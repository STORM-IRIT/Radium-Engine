#ifndef TRIANGLEKDTREE_H
#define TRIANGLEKDTREE_H

#include "Core/Math/bbox.hpp"

#include "Eigen/Core"

#include "Core/Mesh/TriangleMesh.hpp"

#include "Core/Geometry/Triangle/TriangleOperation.hpp"

#include "Core/Geometry/Distance/DistanceQueries.hpp"

#include <limits>
#include <iostream>
#include <numeric>

// max depth of the tree
#define KD_MAX_DEPTH 32

// number of neighbors
#define KD_TRIANGLES_PER_CELL 64

namespace Super4PCS{

template<typename _Index = int >
class TriangleKdTree
{
public:

    typedef _Index  Index;
    typedef std::vector<Index>      IndexList;

    struct KdNode
    {
//        union {
//            struct {
                float splitValue; //value of the dim coordinate
                IndexList triangleIndices;
                unsigned int firstChildId:24;
                unsigned int dim:2; //axis of the split
                unsigned int leaf:1;
//            };
/*            struct { */// if leaf
//                IndexList leafTriangleIndices;
//            };
//        };
    };

    //typedef _Scalar Scalar;
//    typedef _Index  Index;

    static constexpr Index invalidIndex() { return -1; }

    typedef Eigen::Matrix<Scalar,3,1> VectorType;
    typedef Ra::Core::Triangle Triangle;
    typedef AABB3D AxisAlignedBoxType;

    typedef std::vector<KdNode>     NodeList;
    typedef std::vector<VectorType> PointList;
    typedef std::vector<Triangle>   TriangleList;
//    typedef std::vector<Index>      IndexList;

    inline const NodeList&      _getNodes      (void) { return mNodes;      }
    inline const PointList&     _getPoints     (void) { return mPoints;     }
    inline const TriangleList&  _getTriangles  (void) { return mTriangles;  }
    inline const IndexList&     _getIndices    (void) { return mIndices;    }

    TriangleKdTree(const TriangleList& t, const PointList& p,
           unsigned int nofTrianglesPerCell = KD_TRIANGLES_PER_CELL,
           unsigned int maxDepth = KD_MAX_DEPTH );

    TriangleKdTree( unsigned int size = 0,
            unsigned int nofTrianglesPerCell = KD_TRIANGLES_PER_CELL,
            unsigned int maxDepth = KD_MAX_DEPTH );

    inline void finalize();

    inline const AxisAlignedBoxType& aabb() const  {return mAABB; }

    ~TriangleKdTree();

    inline Index doQueryRestrictedClosestIndex(const VectorType& s1, const VectorType& s2, Scalar sqdist);

    inline void doQueryRestrictedClosestIndexes(const VectorType &s1, const VectorType &s2, Scalar sqdist, std::vector<Index>& cl_dist);

    inline Index doQueryRestrictedClosestIndexTriangle(const VectorType &a, const VectorType &b, const VectorType &c);

protected:

    //! element of the stack
    struct QueryNode
    {
        inline QueryNode() {}
        inline QueryNode(unsigned int id) : nodeId(id) {}
        //! id of the next node
        unsigned int nodeId;
        //! squared distance to the next node
        Scalar sq;
    };

    inline void split(unsigned int nodeId/*, IndexList& triangleIndices, unsigned int dim, Scalar splitValue*/);

    void createTree(unsigned int nodeId,
//                    IndexList& triangleIndices,
//                    unsigned int leftchild,
//                    AxisAlignedBoxType bb,
//                    unsigned int d,
                    float splitValue, //splitValue of the father
                    unsigned int level,
                    unsigned int targetCellsize,
                    unsigned int targetMaxDepth);

protected:
    NodeList      mNodes;
    QueryNode mNodeStack[64];
    PointList     mPoints;
    TriangleList  mTriangles;
    IndexList     mIndices;

    AxisAlignedBoxType mAABB;
    unsigned int _nofTrianglesPerCell;
    unsigned int _maxDepth;
};


template<typename Index>
TriangleKdTree<Index>::TriangleKdTree(const TriangleList& t, const PointList& p,
                       unsigned int nofTrianglesPerCell,
                       unsigned int maxDepth)
    : mPoints(p),
      mTriangles(t),
      mIndices(mTriangles.size()),
      mAABB(p.cbegin(), p.cend()),
      _nofTrianglesPerCell(nofTrianglesPerCell),
      _maxDepth(maxDepth)
{
    std::iota (mIndices.begin(), mIndices.end(), 0); // Fill with 0, 1, ..., nbTriangles-1.
    finalize();
}

template<typename Index>
TriangleKdTree<Index>::TriangleKdTree(unsigned int size,
                       unsigned int nofTrianglesPerCell,
                       unsigned int maxDepth)
    : _nofTrianglesPerCell(nofTrianglesPerCell),
      _maxDepth(maxDepth)
{
    mPoints.reserve(size);
    mTriangles.reserve(size);
    mIndices.reserve(size);
}

template<typename Index>
void TriangleKdTree<Index>::finalize()
{
    mNodes.reserve(16 * mTriangles.size() / _nofTrianglesPerCell);
    mNodes.push_back(KdNode());
    mNodes.back().leaf = 0;
    mNodes.back().triangleIndices = mIndices;
    std::cout << "create tree" << std::endl;
    //AxisAlignedBoxType aabb = AxisAlignedBoxType();
    createTree(0, /*mIndices,*/ /*0, aabb, 0,*/ 0, 1, _nofTrianglesPerCell, _maxDepth);
    std::cout << "create tree ... DONE (" << mTriangles.size() << " triangles)" << std::endl;
}

template<typename Index>
TriangleKdTree<Index>::~TriangleKdTree()
{
}

template<typename Index>
Index TriangleKdTree<Index>::doQueryRestrictedClosestIndex(const VectorType &s1, const VectorType &s2, Scalar sqdist)
{
    Index  cl_id   = invalidIndex();
    Scalar cl_dist = sqdist;

    mNodeStack[0].nodeId = 0;
    mNodeStack[0].sq = 0.f;
    unsigned int count = 1;

    //int nbLoop = 0;
    while (count)
    {
        //nbLoop++;
        QueryNode& qnode = mNodeStack[count-1];
        KdNode   & node  = mNodes[qnode.nodeId];

        if (qnode.sq < cl_dist)
        {
            if (node.leaf)
            {
                --count; // pop
//                const int end = node.start+node.size; // no more end, we use the array triangleIndices access all the triangles of the node
//                for (int i=node.start ; i<end ; ++i){
//                    const Scalar sqdist = (queryPoint - mPoints[i]).squaredNorm(); // no more query point but a segment with endpoints s1 ans s2, and nore mPoints but mTriangles
//                    if (sqdist <= cl_dist && mIndices[i] != currentId){ // mIndices was used to order the points, so it's no longer used
//                        cl_dist = sqdist;
//                        cl_id   = mIndices[i];
//                    }
//                }
                const int nbTriangles = node.triangleIndices.size();
                for (int i = 0; i < nbTriangles; i++)
                {
                    const VectorType& segCenter = (Scalar)0.5 * (s1 + s2);
                    const VectorType& segDirection = s2 - s1;
                    Scalar segExtent = (Scalar)0.5 * std::sqrt((s2 - s1).dot(s2 - s1));
                    const VectorType triangle[3] = { mPoints[mTriangles[node.triangleIndices[i]][0]],
                                                     mPoints[mTriangles[node.triangleIndices[i]][1]],
                                                     mPoints[mTriangles[node.triangleIndices[i]][2]] };
                    const Scalar sqdist = Ra::Core::DistanceQueries::segmentToTriSq(segCenter, segDirection, segExtent, triangle).sqrDistance;
                    if (sqdist < cl_dist) // < is enough
                    {
                        cl_dist = sqdist;
                        cl_id = node.triangleIndices[i];
                    }
                }
            }
//            else
//            {
//                // replace the stack top by the farthest and push the closest
////                const Scalar new_off = queryPoint[node.dim] - node.splitValue; // again, no more query point but a segment

//                //const Scalar new_off = Ra::Core::DistanceQueries::pointToSegmentSq(node.splitValue, s1, s2 - s1); //distance between a segment and a line

//                const VectorType &s = (s1 + s2) / 2;

//                const Scalar new_off = s[node.dim] - node.splitValue;

//                //std::cout << "new_off = " << new_off << std::endl;

//                // useless because we'll be going through all the nodes anyway
//                if (new_off < 0.)
//                {
//                    mNodeStack[count].nodeId  = node.firstChildId; // stack top the farthest
//                    qnode.nodeId = node.firstChildId+1;            // push the closest
//                }
//                else
//                {
//                    mNodeStack[count].nodeId  = node.firstChildId+1;
//                    qnode.nodeId = node.firstChildId;
//                }
//                mNodeStack[count].sq = qnode.sq;
//                qnode.sq = new_off*new_off;
//                ++count;
//            }
            else
            {
                Scalar new_off;
                const Scalar d1 = s1[node.dim] - node.splitValue;
                const Scalar d2 = s2[node.dim] - node.splitValue;
                // the segment is on the left side of the split plane
                if (d1 < 0. && d2 < 0.)
                {
                    if (d1 >= d2)
                    {
                        new_off = d1;
                    }
                    else
                    {
                        new_off = d2;
                    }
                    mNodeStack[count].nodeId  = node.firstChildId;
                    qnode.nodeId = node.firstChildId+1;
                }
                // the segment is on the right side of the split plane
                else if (d1 >= 0. && d2 >= 0.)
                {
                    if (d1 <= d2)
                    {
                        new_off = d1;
                    }
                    else
                    {
                        new_off = d2;
                    }
                    mNodeStack[count].nodeId  = node.firstChildId+1;
                    qnode.nodeId = node.firstChildId;
                }
                // the segment is intersecting the split plane
                else
                {
                    new_off = 0;
                    const VectorType &s = (s1 + s2) / 2;
                    const Scalar d = s[node.dim] - node.splitValue;
                    if (d < 0.)
                    {
                        mNodeStack[count].nodeId  = node.firstChildId; // stack top the farthest
                        qnode.nodeId = node.firstChildId+1;            // push the closest
                    }
                    else
                    {
                        mNodeStack[count].nodeId  = node.firstChildId+1;
                        qnode.nodeId = node.firstChildId;
                    }
                }
                mNodeStack[count].sq = qnode.sq;
                qnode.sq = new_off*new_off;
                ++count;
            }
        }
        else
        {
            // pop
            --count;
        }
    }
    return cl_id;
}

template<typename Index>
void TriangleKdTree<Index>::doQueryRestrictedClosestIndexes(const VectorType &s1, const VectorType &s2, Scalar sqdist, std::vector<Index>& cl_id)
{
    mNodeStack[0].nodeId = 0;
    mNodeStack[0].sq = 0.f;
    unsigned int count = 1;

    //int nbLoop = 0;
    while (count)
    {
        //nbLoop++;
        QueryNode& qnode = mNodeStack[count-1];
        KdNode   & node  = mNodes[qnode.nodeId];

        if (qnode.sq < sqdist)
        {
            if (node.leaf)
            {
                --count; // pop
                const int nbTriangles = node.triangleIndices.size();
                for (int i = 0; i < nbTriangles; i++)
                {
                    const VectorType& segCenter = (Scalar)0.5 * (s1 + s2);
                    const VectorType& segDirection = s2 - s1;
                    Scalar segExtent = (Scalar)0.5 * std::sqrt((s2 - s1).dot(s2 - s1));
                    const VectorType triangle[3] = { mPoints[mTriangles[node.triangleIndices[i]][0]],
                                                     mPoints[mTriangles[node.triangleIndices[i]][1]],
                                                     mPoints[mTriangles[node.triangleIndices[i]][2]] };
                    const Scalar dist = Ra::Core::DistanceQueries::segmentToTriSq(segCenter, segDirection, segExtent, triangle).sqrDistance;
                    if (dist < sqdist)
                    {
                        cl_id.push_back(node.triangleIndices[i]);
                    }
                }
            }
            else
            {
                Scalar new_off;
                const Scalar d1 = s1[node.dim] - node.splitValue;
                const Scalar d2 = s2[node.dim] - node.splitValue;
                // the segment is on the left side of the split plane
                if (d1 < 0. && d2 < 0.)
                {
                    if (d1 >= d2)
                    {
                        new_off = d1;
                    }
                    else
                    {
                        new_off = d2;
                    }
                    mNodeStack[count].nodeId  = node.firstChildId;
                    qnode.nodeId = node.firstChildId+1;
                }
                // the segment is on the right side of the split plane
                else if (d1 >= 0. && d2 >= 0.)
                {
                    if (d1 <= d2)
                    {
                        new_off = d1;
                    }
                    else
                    {
                        new_off = d2;
                    }
                    mNodeStack[count].nodeId  = node.firstChildId+1;
                    qnode.nodeId = node.firstChildId;
                }
                // the segment is intersecting the split plane
                else
                {
                    new_off = 0;
                    const VectorType &s = (s1 + s2) / 2;
                    const Scalar d = s[node.dim] - node.splitValue;
                    if (d < 0.)
                    {
                        mNodeStack[count].nodeId  = node.firstChildId; // stack top the farthest
                        qnode.nodeId = node.firstChildId+1;            // push the closest
                    }
                    else
                    {
                        mNodeStack[count].nodeId  = node.firstChildId+1;
                        qnode.nodeId = node.firstChildId;
                    }
                }
                mNodeStack[count].sq = qnode.sq;
                qnode.sq = new_off*new_off;
                ++count;
            }
        }
        else
        {
            // pop
            --count;
        }
    }
}

template<typename Index>
Index TriangleKdTree<Index>::doQueryRestrictedClosestIndexTriangle(const VectorType &a, const VectorType &b, const VectorType &c)
{
    Index cl_id = invalidIndex();
    Scalar cl_dist = std::numeric_limits<Scalar>::max();

    mNodeStack[0].nodeId = 0;
    mNodeStack[0].sq = 0.f;
    unsigned int count = 1;

    while (count)
    {
        QueryNode& qnode = mNodeStack[count-1];
        KdNode   & node  = mNodes[qnode.nodeId];

        if (qnode.sq < cl_dist)
        {
            if (node.leaf)
            {
                --count; // pop
                const int nbTriangles = node.triangleIndices.size();
                for (int i = 0; i < nbTriangles; i++)
                {
                    const VectorType v[3] = {a,b,c};
                    const VectorType triangle[3] = { mPoints[mTriangles[node.triangleIndices[i]][0]],
                                                     mPoints[mTriangles[node.triangleIndices[i]][1]],
                                                     mPoints[mTriangles[node.triangleIndices[i]][2]] };
                    const Scalar sqdist = Ra::Core::DistanceQueries::triangleToTriSq(v, triangle).sqrDistance;
                    if (sqdist < cl_dist)
                    {
                        cl_dist = sqdist;
                        cl_id = node.triangleIndices[i];
                    }
                }
            }

            else
            {
                Scalar new_off;
                const Scalar d1 = a[node.dim] - node.splitValue;
                const Scalar d2 = b[node.dim] - node.splitValue;
                const Scalar d3 = c[node.dim] - node.splitValue;
                // the triangle is on the left side of the split plane
                if (d1 < 0. && d2 < 0. && d3 < 0.)
                {
                    if (d1 >= d2 && d1 >= d3)
                    {
                        new_off = d1;
                    }
                    else
                    {
                        if (d2 >= d3)
                        {
                            new_off = d2;
                        }
                        else
                        {
                            new_off = d3;
                        }
                    }
                    mNodeStack[count].nodeId  = node.firstChildId;
                    qnode.nodeId = node.firstChildId+1;
                }
                // the triangle is on the right side of the split plane
                else if (d1 >= 0. && d2 >= 0. && d3 >= 0.)
                {
                    if (d1 <= d2 && d1 <= d3)
                    {
                        new_off = d1;
                    }
                    else
                    {
                        if (d2 <= d3)
                        {
                            new_off = d2;
                        }
                        else
                        {
                            new_off = d3;
                        }
                    }
                    mNodeStack[count].nodeId  = node.firstChildId+1;
                    qnode.nodeId = node.firstChildId;
                }
                // the segment is intersecting the split plane
                else
                {
                    new_off = 0;
                    const VectorType &v = Ra::Core::Geometry::triangleBarycenter(a,b,c);
                    const Scalar d = v[node.dim] - node.splitValue;
                    if (d < 0.)
                    {
                        mNodeStack[count].nodeId  = node.firstChildId; // stack top the farthest
                        qnode.nodeId = node.firstChildId+1;            // push the closest
                    }
                    else
                    {
                        mNodeStack[count].nodeId  = node.firstChildId+1;
                        qnode.nodeId = node.firstChildId;
                    }
                }
                mNodeStack[count].sq = qnode.sq;
                qnode.sq = new_off*new_off;
                ++count;
            }
        }
        else
        {
            // pop
            --count;
        }
    }
    return cl_id;
}

template<typename Index>
void TriangleKdTree<Index>::split(unsigned int nodeId/*, IndexList& triangleIndices, unsigned int dim, Scalar splitValue*/) //equal number of triangles in both sides are needed
{
    KdNode& node = mNodes[nodeId];

    for (unsigned int i = 0; i < node.triangleIndices.size(); i++)
    {
        if (mPoints[mTriangles[node.triangleIndices[i]][0]][node.dim] < node.splitValue || mPoints[mTriangles[node.triangleIndices[i]][1]][node.dim] < node.splitValue || mPoints[mTriangles[node.triangleIndices[i]][2]][node.dim] < node.splitValue)
        {
            if (mPoints[mTriangles[node.triangleIndices[i]][0]][node.dim] < node.splitValue && mPoints[mTriangles[node.triangleIndices[i]][1]][node.dim] < node.splitValue && mPoints[mTriangles[node.triangleIndices[i]][2]][node.dim] < node.splitValue)
            {
                mNodes[node.firstChildId].triangleIndices.push_back(node.triangleIndices[i]);
            }
            else
            {
                mNodes[node.firstChildId].triangleIndices.push_back(node.triangleIndices[i]);
                mNodes[node.firstChildId+1].triangleIndices.push_back(node.triangleIndices[i]);
            }
        }
        else
        {
            mNodes[node.firstChildId+1].triangleIndices.push_back(node.triangleIndices[i]);
        }
    }
}

template<typename Index>
void TriangleKdTree<Index>::createTree(unsigned int nodeId, /*IndexList& triangleIndices,*/ /*unsigned int leftchild, AxisAlignedBoxType bb, unsigned int d,*/ float splitValue, unsigned int level, unsigned int targetCellSize, unsigned int targetMaxDepth)
{
    KdNode& node = mNodes[nodeId];
    AxisAlignedBoxType aabb = AxisAlignedBoxType();

    std::cout << "level : " << level << std::endl;

    const uint nbTriangles = node.triangleIndices.size();
    const uint nbPoints = 3 * nbTriangles;
    PointList AABBPoints; //to compute the aabbox
    AABBPoints.reserve(nbPoints);
    for (unsigned int i = 0; i < nbTriangles; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            AABBPoints.push_back(mPoints[mTriangles[node.triangleIndices[i]][j]]);
        }
    }
    for (unsigned int k = 0; k< AABBPoints.size(); k++)
    {
        aabb.extendTo(AABBPoints[k]);
    }

//    for (unsigned int i = 0; i < triangleIndices.size(); i++)
//    {
//        for (unsigned int j = 0; j < 3; j++)
//        {
//            aabb.extendTo(mPoints[mTriangles[triangleIndices[i]][j]]);
//        }
//    }

//    if (nodeId == 0)
//    {
//        for (unsigned int i = 0; i < mPoints.size(); i++)
//        {
//            aabb.extendTo(mPoints[i]);
//        }
//    }
//    else
//    {
//        VectorType aabb_min = bb.min();
//        VectorType aabb_max = bb.max();
//        if (leftchild == 1)
//        {
//            aabb_max(d) = splitValue;
//        }
//        else
//        {
//            aabb_min(d) = splitValue;
//        }
//        aabb = AxisAlignedBoxType(aabb_min, aabb_max);
//    }

    VectorType diag =  Scalar(0.5) * (aabb.max()- aabb.min()); //middle of the aabbox
    typename VectorType::Index dim;

    diag.maxCoeff(&dim); // finding the longest axis

    node.dim = dim;
    node.splitValue = aabb.center()(dim); // spliting the longest axis

    if (node.splitValue == splitValue && nodeId != 0)
    {
        node.leaf = 1;
    }
    else
    {

    node.firstChildId = mNodes.size();

    {
        KdNode n;
        mNodes.push_back(n);
        mNodes.push_back(n);
    }

    split(nodeId/*, node.triangleIndices, dim, node.splitValue*/); //sorting the triangles in regard to the split value of the selected axis (left and right)

//    node.firstChildId = mNodes.size();

//    {
//        KdNode n;
//        mNodes.push_back(n);
//        mNodes.push_back(n);
//    }
    //mNodes << Node() << Node();
    //mNodes.resize(mNodes.size()+2);

    int matches = 0;
    for (unsigned int i = 0; i < mNodes[node.firstChildId].triangleIndices.size(); i++)
    {
        for (unsigned int j = 0; j < mNodes[node.firstChildId+1].triangleIndices.size(); j++)
        {
            if (mNodes[node.firstChildId].triangleIndices[i] == mNodes[node.firstChildId+1].triangleIndices[j])
                matches++;
        }
    }

    if (matches > 0 && (mNodes[node.firstChildId].triangleIndices.size() / matches <= 1 && mNodes[node.firstChildId+1].triangleIndices.size() / matches <= 1))
    {
        mNodes[node.firstChildId].leaf = 1;
        mNodes[node.firstChildId+1].leaf = 1;
    }

    else
    {

    {
        // left child
        unsigned int childId = node.firstChildId;
        KdNode& child = mNodes[childId];
        if (child.triangleIndices.size() <= targetCellSize || level>=targetMaxDepth/* || child.triangleIndices.size() / matches <= 1*/)
        {
            child.leaf = 1;
//            child.leafTriangleIndices = child.triangleIndices;
        }
        else
        {
            child.leaf = 0;
            createTree(childId, /*child.triangleIndices,*/ /*1, aabb, node.dim,*/ node.splitValue, level+1, targetCellSize, targetMaxDepth);
        }
    }

    {
        // right child
        unsigned int childId = node.firstChildId+1;
        KdNode& child = mNodes[childId];
        if (child.triangleIndices.size() <= targetCellSize || level>=targetMaxDepth/* || child.triangleIndices.size() / matches <= 1*/)
        {
            child.leaf = 1;
//            child.leafTriangleIndices = child.triangleIndices;
        }
        else
        {
            child.leaf = 0;
            createTree(childId, /*child.triangleIndices,*/ /*0, aabb, node.dim,*/ node.splitValue, level+1, targetCellSize, targetMaxDepth);
        }
    }
    }
    }
}

} //namespace Super4PCS

#endif // TRIANGLEKDTREE_H

