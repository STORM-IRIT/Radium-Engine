#ifndef RADIUMENGINE_MESH_PRIMITIVES_HPP_
#define RADIUMENGINE_MESH_PRIMITIVES_HPP_

#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {
//
// Primitive construction
//

/// Create a 2D grid mesh with given number of row and columns
RA_CORE_API TriangleMesh makePlaneGrid( const uint rows = 1, const uint cols = 1,
                                        const Math::Vector2& halfExts = Math::Vector2( 0.5, 0.5 ),
                                        const Math::Transform& T = Math::Transform::Identity() );

/// Create a 2D quad mesh given half extents, centered on the origin with x axis as normal
RA_CORE_API TriangleMesh makeXNormalQuad( const Math::Vector2& halfExts = Math::Vector2( 0.5, 0.5 ) );

/// Create a 2D quad mesh given half extents, centered on the origin with y axis as normal
RA_CORE_API TriangleMesh makeYNormalQuad( const Math::Vector2& halfExts = Math::Vector2( 0.5, 0.5 ) );

/// Create a 2D quad mesh given half extents, centered on the origin with z axis as normal
RA_CORE_API TriangleMesh makeZNormalQuad( const Math::Vector2& halfExts = Math::Vector2( 0.5, 0.5 ) );

/// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
RA_CORE_API TriangleMesh makeBox( const Math::Vector3& halfExts = Math::Vector3( 0.5, 0.5, 0.5 ) );

/// Create an axis-aligned cubic mesh
RA_CORE_API TriangleMesh makeBox( const Math::Aabb& aabb );

/// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
RA_CORE_API TriangleMesh makeSharpBox( const Math::Vector3& halfExts = Math::Vector3( 0.5, 0.5, 0.5 ) );

/// Create an axis-aligned cubic mesh
RA_CORE_API TriangleMesh makeSharpBox( const Math::Aabb& aabb );

/// Create a parametric spherical mesh of given radius. Template parameters set the resolution.
template <uint U = 16, uint V = U>
TriangleMesh makeParametricSphere( Scalar radius = 1.0 );

/// Create a parametric torus mesh. The minor radius is the radius of the inside of the tube and the
/// major radius is the radius of the whole torus. The torus will be centered at the origin and have
/// Z as rotation axis. Template parameters set the resolution of the mesh.
template <uint U = 16, uint V = U>
TriangleMesh makeParametricTorus( Scalar majorRadius, Scalar minorRadius );

/// Create a spherical mesh by subdivision of an icosahedron.
RA_CORE_API TriangleMesh makeGeodesicSphere( Scalar radius = 1.f, uint numSubdiv = 3 );

/// Create a cylinder approximation (n-faced prism) with base faces centered on A and B
/// with given radius.
RA_CORE_API TriangleMesh makeCylinder( const Math::Vector3& a, const Math::Vector3& b, Scalar radius,
                                       uint nFaces = 32 );

/// Create a capsule with given cylinder length and radius.
/// Total length is length + 2*radius
/// The capsule is along z axis
RA_CORE_API TriangleMesh makeCapsule( Scalar length, Scalar radius, uint nFaces = 32 );

/// Create a tube (empty cylinder) delimited by two radii, with bases centered on A and B.
/// Outer radius must be larger than inner radius.
RA_CORE_API TriangleMesh makeTube( const Math::Vector3& a, const Math::Vector3& b, Scalar outerRadius,
                                   Scalar InnerRadius, uint nFaces = 32 );

/// Create a cone approximation (n-faced pyramid) with base face centered on base, pointing
/// towards tip with given base radius.
RA_CORE_API TriangleMesh makeCone( const Math::Vector3& base, const Math::Vector3& tip, Scalar radius,
                                   uint nFaces = 32 );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/MeshPrimitives.inl>
#endif // RADIUMENGINE_MESH_PRIMITIVES_HPP_
