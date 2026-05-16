#pragma once

#include <Core/Geometry/AttribArrayGeometry.hpp>
#include <Core/Geometry/IndexedGeometry.hpp>
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
/// \param rows: number of rows (y direction)
/// \param cols: number of columns (x direction)
/// \param halfExts: from center of the grid to each corner, in 2D, before applying T
/// \param T: apply T to the generated vertices
/// \param color: if set, colorize vertices
/// \param generateTexCoord: create uv tex coordinates on the grid, -halfExts have uv = (0,0),
/// +halfExts have uv=(1,1)
RA_CORE_API QuadMesh makePlaneGrid( const uint rows         = 1,
                                    const uint cols         = 1,
                                    const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                                    const Transform& T      = Transform::Identity(),
                                    const Utils::optional<Utils::Color>& color = {},
                                    bool generateTexCoord                      = false );

/// Create a 2D quad mesh given half extents, centered on the origin with x axis as normal
/// \see makePlaneGrid
RA_CORE_API QuadMesh makeXNormalQuad( const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                                      const Utils::optional<Utils::Color>& color = {},
                                      bool generateTexCoord                      = false );

/// Create a 2D quad mesh given half extents, centered on the origin with y axis as normal
/// \see makePlaneGrid
RA_CORE_API QuadMesh makeYNormalQuad( const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                                      const Utils::optional<Utils::Color>& color = {},
                                      bool generateTexCoord                      = false );

/// Create a 2D quad mesh given half extents, centered on the origin with z axis as normal
/// \see makePlaneGrid
RA_CORE_API QuadMesh makeZNormalQuad( const Vector2& halfExts = Vector2( .5_ra, .5_ra ),
                                      const Utils::optional<Utils::Color>& color = {},
                                      bool generateTexCoord                      = false );

/// Create a 2D grid, with only outer points, connected as lines, inner line intersctions are not
/// present in verstices.
RA_CORE_API MultiIndexedGeometry makeGrid( const Core::Vector3& center,
                                           const Core::Vector3& x,
                                           const Core::Vector3& y,
                                           const Core::Utils::Color& color,
                                           Scalar cell_size,
                                           uint res );

/// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
RA_CORE_API MultiIndexedGeometry makeBox2( const Vector3& halfExts = Vector3( .5_ra, .5_ra, .5_ra ),
                                           const Utils::optional<Utils::Color>& color = {} );

/// Create an axis-aligned cubic mesh
RA_CORE_API MultiIndexedGeometry makeBox2( const Aabb& aabb,
                                           const Utils::optional<Utils::Color>& color = {} );

RA_CORE_API MultiIndexedGeometry makeBox2( const Vector3& corner,
                                           const Vector3& x,
                                           const Vector3& y,
                                           const Vector3& z,
                                           const Utils::optional<Utils::Color>& color );

/// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
RA_CORE_API MultiIndexedGeometry makeSharpBox2( const Vector3& halfExts = Vector3( .5_ra,
                                                                                   .5_ra,
                                                                                   .5_ra ),
                                                const Utils::optional<Utils::Color>& color = {},
                                                bool generateTexCoord = false );

/// Create an axis-aligned cubic mesh
RA_CORE_API MultiIndexedGeometry makeSharpBox2( const Aabb& aabb,
                                                const Utils::optional<Utils::Color>& color = {},
                                                bool generateTexCoord = false );

/// Create a parametric spherical mesh of given radius. Template parameters set the resolution.
/// \param generateTexCoord: maps parametric (u,v) to texture corrdinates [0,1]^2
RA_CORE_API MultiIndexedGeometry
makeParametricSphere( Scalar radius                              = 1_ra,
                      const Utils::optional<Utils::Color>& color = {},
                      bool generateTexCoord                      = false,
                      const uint SLICES                          = 16,
                      const uint STACK                           = 16 );

/// Create a parametric torus mesh. The minor radius is the radius of the inside of the tube and
/// the major radius is the radius of the whole torus. The torus will be centered at the origin
/// and have Z as rotation axis. Template parameters set the resolution of the mesh. \param
/// generateTexCoord: maps parametric (u,v) to texture corrdinates [0,1]^2
RA_CORE_API MultiIndexedGeometry
makeParametricTorus( Scalar majorRadius,
                     Scalar minorRadius,
                     const Utils::optional<Utils::Color>& color = {},
                     bool generateTexCoord                      = false,
                     const uint U                               = 16,
                     const uint V                               = 16 );

/// Create a spherical mesh by subdivision of an icosahedron.
RA_CORE_API MultiIndexedGeometry
makeGeodesicSphere( Scalar radius                              = 1_ra,
                    uint numSubdiv                             = 3,
                    const Utils::optional<Utils::Color>& color = {} );

/// Create a cylinder approximation (sideSegments-faced prism) with base faces centered on a and
/// b with given radius. Fill (the tube part) is split into equally sapced fill segments. Side
/// and fill make a sharp edge.
RA_CORE_API
MultiIndexedGeometry makeCylinder( const Vector3& a,
                                   const Vector3& b,
                                   Scalar radius,
                                   uint sideSegments                          = 32,
                                   uint fillSegments                          = 2,
                                   const Utils::optional<Utils::Color>& color = {} );

/// Create a capsule with given cylinder length and radius.
/// Total length is length + 2*radius
/// The capsule is along z axis
RA_CORE_API MultiIndexedGeometry makeCapsule( Scalar length,
                                              Scalar radius,
                                              uint nFaces                                = 32,
                                              const Utils::optional<Utils::Color>& color = {} );

/// Create a tube (empty cylinder) delimited by two radii, with bases centered on A and B.
/// Outer radius must be larger than inner radius.
RA_CORE_API MultiIndexedGeometry makeTube( const Vector3& a,
                                           const Vector3& b,
                                           Scalar outerRadius,
                                           Scalar InnerRadius,
                                           uint nFaces                                = 32,
                                           const Utils::optional<Utils::Color>& color = {} );

/// Create a cone approximation (n-faced pyramid) with base face centered on base, pointing
/// towards tip with given base radius.
RA_CORE_API MultiIndexedGeometry makeCone( const Vector3& base,
                                           const Vector3& tip,
                                           Scalar radius,
                                           uint nFaces                                = 32,
                                           const Utils::optional<Utils::Color>& color = {} );

} // namespace Geometry
} // namespace Core
} // namespace Ra
