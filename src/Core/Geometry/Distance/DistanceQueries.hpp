#ifndef RADIUMENGINE_DISTANCE_QUERIES_HPP_
#define RADIUMENGINE_DISTANCE_QUERIES_HPP_

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/RaCore.hpp>
#include <limits>

/// Functions in this file are utilities to compute the distance between various geometric sets.
/// They always return the squared distance.
/// References : Geometric Tools, https://github.com/pmjoniak/GeometricTools \n
///              Wolfram, http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html \n
///              Fast Distance Queries for Triangles, Lines and Points using SSE instructions
///              Evan Shellshear, Robin Ytterlid, Journal of Computer Graphics Techniques, Vol.3 No.
///              4 (2014), http://jcgt.org/published/0003/04/05/paper.pdf
namespace Ra {
namespace Core {
namespace DistanceQueries {
//
// Point-to-line distance
//

/// Return the squared distance from point Q to the line defined by point A and direction dir.
inline RA_CORE_API Scalar pointToLineSq( const Vector3& q, const Vector3& a, const Vector3& dir );

/// Project point Q on segment AB defined by point A and vector AB = (B -A).
/// Return the parameter t in [0,1] which identifies the projected point.
inline RA_CORE_API Scalar projectOnSegment( const Vector3& q, const Vector3& a, const Vector3& ab );

/// Return the squared distance from point Q to the segment AB defined by point A and
/// vector AB = (B - A).
inline RA_CORE_API Scalar pointToSegmentSq( const Vector3& q, const Vector3& a, const Vector3& ab );

//
// Point-to-triangle distance
//

/// Structure holding the result of a point-to triangle distance query.
struct PointToTriangleOutput {
    /// Flags denoting the primitive where the closest point is.
    enum Flags { HIT_FACE = 0, HIT_VERTEX = 1, HIT_EDGE = 2 };

    PointToTriangleOutput() :
        meshPoint( {0, 0, 0} ),
        distanceSquared( std::numeric_limits<Scalar>::max() ),
        flags( 0 ) {}

    /// The point hit on the mesh.
    Vector3 meshPoint;

    /// The squared distance to the point.
    Scalar distanceSquared;

    /// The flags for the closest primitive.
    /// Bits 0-1 : if the hit is a face (00) a vertex (01) or an edge (10)
    /// bits 2-3 : if vertex, index of the hit vertex
    ///            if edge, index of the edge's first vertex
    uchar flags;

    /// Return if the hit is a face, a vertex or an edge
    inline Flags getHitPrimitive() const { return Flags( flags & 0x3u ); }

    /// Return the index of the hit vertex or the first vertex of the hit edge.
    /// If hit is a face hit, returns 0
    uint getHitIndex() const { return ( flags & 0xcu ) >> 2; }
};

/// Computes the squared distance from a query point Q to the triangle ABC.
inline RA_CORE_API PointToTriangleOutput pointToTriSq( const Vector3& q, const Vector3& a,
                                                       const Vector3& b, const Vector3& c );

//
// Line-to-segment distance
//

/// Structure holding the result of a line-to-segment distance query.
struct LineToSegmentOutput {
    Scalar distance;         ///< Distance between the primitives.
    Scalar sqrDistance;      ///< Squared distance.
    Scalar parameter[2];     ///< Curvilinear parameters of the closest points
                             ///< for each primitive. The line comes first.
    Vector3 closestPoint[2]; ///< The closest points on each primitive.
                             ///< The line comes first.
};

/// Computes the squared distance from a query line to a segment.
inline RA_CORE_API LineToSegmentOutput lineToSegSq( const Vector3& lineOrigin,
                                                    Vector3 lineDirection, const Vector3& segCenter,
                                                    Vector3 segDirection, const Scalar segExtent );

//
// Line-to-triangle distance
//

/// Structure holding the result of a line-to-triangle distance query.
struct LineToTriangleOutput {
    Scalar distance;             ///< Distance between the primitives.
    Scalar sqrDistance;          ///< Squared distance.
    Scalar lineParameter;        ///< Curvilinear parameter of the closest point on the line.
    Scalar triangleParameter[3]; ///< Barycentric coordinates of the closest point on the triangles.
    Vector3 closestPoint[2];     ///< The closest points on each primitive.
                                 ///< The line comes first.
};

/// Computes the squared distance from a query line to the triangle v.
inline RA_CORE_API LineToTriangleOutput lineToTriSq( const Vector3& lineOrigin,
                                                     Vector3 lineDirection, const Vector3 v[3] );

//
// Segment-to-triangle distance
//

/// Structure holding the result of a segment-to-triangle distance query.
struct SegmentToTriangleOutput {
    Scalar distance;             ///< Distance between the primitives.
    Scalar sqrDistance;          ///< Squared distance.
    Scalar segmentParameter;     ///< Curvilinear parameter of the closest point on the segment.
    Scalar triangleParameter[3]; ///< Barycentri coordinates of the closest point on the triangle.
    Vector3 closestPoint[2];     ///< The closest points on each primitive.
                                 ///< The segment comes first.
};

/// Computes the squared distance from a query segment to the triangle v.
inline RA_CORE_API SegmentToTriangleOutput segmentToTriSq( const Vector3& segCenter,
                                                           Vector3 segDirection,
                                                           const Scalar segExtent,
                                                           const Vector3 v[3] );

//
// Triangle-to-triangle distance
//

/// Structure holding the result of a triangle-to-triangle distance query.
struct TriangleToTriangleOutput {
    Scalar distance;              ///< Distance between the primitives.
    Scalar sqrDistance;           ///< Squared Distance.
    Scalar triangleParameter1[3]; ///< Barycentric coordinates of the closest point on the first
                                  ///< triangle.
    Scalar triangleParameter2[3]; ///< Barycentric coordinates of the closest point on the first
                                  ///< triangle.
    Vector3 closestPoint[2];      ///< The closest pionts on each triangle.
                                  ///< The first triangle comes first.
};

/// Computes the squared distance from a query triangle v1 to the triangle v2.
inline RA_CORE_API TriangleToTriangleOutput triangleToTriSq( const Vector3 v1[3],
                                                             const Vector3 v2[3] );

} // namespace DistanceQueries
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Distance/DistanceQueries.inl>

#endif // RADIUMENGINE_DISTANCE_QUERIES_HPP_
