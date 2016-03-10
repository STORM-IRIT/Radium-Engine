#ifndef RADIUMENGINE_MESH_PRIMITIVES_HPP_
#define RADIUMENGINE_MESH_PRIMITIVES_HPP_

#include <Core/RaCore.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra
{
    namespace Core
    {
        namespace MeshUtils
        {
            //
            // Primitive construction
            //

            /// Create a 2D grid mesh with given number of row and columns
            RA_CORE_API TriangleMesh makePlaneGrid( const uint rows = 1, const uint cols = 1,
                const Vector2& halfExts = Vector2( 0.5, 0.5 ), const Transform& T = Transform::Identity() );

            /// Create a 2D quad mesh given half extents, centered on the origin with x axis as normal
            RA_CORE_API TriangleMesh makeXNormalQuad( const Vector2& halfExts = Vector2( 0.5, 0.5 ) );

            /// Create a 2D quad mesh given half extents, centered on the origin with y axis as normal
            RA_CORE_API TriangleMesh makeYNormalQuad( const Vector2& halfExts = Vector2( 0.5, 0.5 ) );

            /// Create a 2D quad mesh given half extents, centered on the origin with z axis as normal
            RA_CORE_API TriangleMesh makeZNormalQuad( const Vector2& halfExts = Vector2( 0.5, 0.5 ) );

            /// Create an axis-aligned cubic mesh with the given half extents, centered on the origin.
            RA_CORE_API TriangleMesh makeBox( const Vector3& halfExts = Vector3( 0.5, 0.5, 0.5 ) );

            /// Create an axis-aligned cubic mesh
            RA_CORE_API TriangleMesh makeBox( const Aabb& aabb );

            /// Create a parametric spherical mesh of given radius. Template parameters set the resolution.
            template<uint U = 16, uint V = U>
            TriangleMesh makeParametricSphere( Scalar radius = 1.0 );

            /// Create a parametric torus mesh. The minor radius is the radius of the inside of the tube and the major
            /// radius is the radius of the whole torus. The torus will be centered at the origin and have Z as rotation axis.
            /// Template parameters set the resolution of the mesh.
            template<uint U = 16, uint V = U>
            TriangleMesh makeParametricTorus( Scalar majorRadius, Scalar minorRadius );

            /// Create a spherical mesh by subdivision of an icosahedron.
            RA_CORE_API TriangleMesh makeGeodesicSphere( Scalar radius = 1.f, uint numSubdiv = 3 );

            /// Create a cylinder approximation (n-faced prism) with base faces centered on A and B
            /// with given radius.
            RA_CORE_API TriangleMesh makeCylinder( const Vector3& a, const Vector3& b, Scalar radius, uint nFaces = 32);

            /// Create a tube (empty cylinder) delimited by two radii, with bases centered on A and B.
            /// Outer radius must be larger than inner radius.
            RA_CORE_API TriangleMesh makeTube( const Vector3& a, const Vector3& b, Scalar outerRadius, Scalar InnerRadius, uint nFaces = 32);

            /// Create a cone approximation (n-faced pyramid) with base face centered on base, pointing
            /// towards tip with given base radius.
            RA_CORE_API TriangleMesh makeCone( const Vector3& base, const Vector3& tip, Scalar radius, uint nFaces = 32);

        }  // Mesh Utils
    } // Core
} // Ra

#include <Core/Mesh/MeshPrimitives.inl>
#endif // RADIUMENGINE_MESH_PRIMITIVES_HPP_
