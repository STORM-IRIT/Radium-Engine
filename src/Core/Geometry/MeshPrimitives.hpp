#ifndef RADIUMENGINE_MESH_PRIMITIVES_HPP_
#define RADIUMENGINE_MESH_PRIMITIVES_HPP_

#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/StdOptional.hpp>

namespace Ra {
namespace Core {
namespace Geometry {
//
// Primitive construction
//

/// Create a 2D grid mesh with given number of row and columns
RA_CORE_API TriangleMesh makePlaneGrid(
    const uint rows = 1, const uint cols = 1, const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
    const Transform& T = Transform::Identity(),
    const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a 2D quad mesh given half extents, centered on the origin with x axis as normal
RA_CORE_API TriangleMesh
makeXNormalQuad( const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                 const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a 2D quad mesh given half extents, centered on the origin with y axis as normal
RA_CORE_API TriangleMesh
makeYNormalQuad( const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                 const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a 2D quad mesh given half extents, centered on the origin with z axis as normal
RA_CORE_API TriangleMesh
makeZNormalQuad( const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                 const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
RA_CORE_API TriangleMesh makeBox( const Vector3& halfExts = Vector3( .5_ra, .5_ra, .5_ra ),
                                  const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create an axis-aligned cubic mesh
RA_CORE_API TriangleMesh makeBox( const Aabb& aabb,
                                  const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
RA_CORE_API TriangleMesh
makeSharpBox( const Vector3& halfExts = Vector3( .5_ra, .5_ra, .5_ra ),
              const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create an axis-aligned cubic mesh
RA_CORE_API TriangleMesh
makeSharpBox( const Aabb& aabb, const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a parametric spherical mesh of given radius. Template parameters set the resolution.
template <uint U = 16, uint V = U>
TriangleMesh makeParametricSphere( Scalar radius = 1_ra,
                                   const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a parametric torus mesh. The minor radius is the radius of the inside of the tube and the
/// major radius is the radius of the whole torus. The torus will be centered at the origin and have
/// Z as rotation axis. Template parameters set the resolution of the mesh.
template <uint U = 16, uint V = U>
TriangleMesh makeParametricTorus( Scalar majorRadius, Scalar minorRadius,
                                  const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a spherical mesh by subdivision of an icosahedron.
RA_CORE_API TriangleMesh
makeGeodesicSphere( Scalar radius = 1_ra, uint numSubdiv = 3,
                    const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a cylinder approximation (n-faced prism) with base faces centered on A and B
/// with given radius.
RA_CORE_API TriangleMesh
makeCylinder( const Vector3& a, const Vector3& b, Scalar radius, uint nFaces = 32,
              const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a capsule with given cylinder length and radius.
/// Total length is length + 2*radius
/// The capsule is along z axis
RA_CORE_API TriangleMesh makeCapsule( Scalar length, Scalar radius, uint nFaces = 32,
                                      const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a tube (empty cylinder) delimited by two radii, with bases centered on A and B.
/// Outer radius must be larger than inner radius.
RA_CORE_API TriangleMesh makeTube( const Vector3& a, const Vector3& b, Scalar outerRadius,
                                   Scalar InnerRadius, uint nFaces = 32,
                                   const Utils::optional<Utils::Color>& color = Utils::Color() );

/// Create a cone approximation (n-faced pyramid) with base face centered on base, pointing
/// towards tip with given base radius.
RA_CORE_API TriangleMesh makeCone( const Vector3& base, const Vector3& tip, Scalar radius,
                                   uint nFaces = 32,
                                   const Utils::optional<Utils::Color>& color = Utils::Color() );

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/MeshPrimitives.inl>
#endif // RADIUMENGINE_MESH_PRIMITIVES_HPP_
