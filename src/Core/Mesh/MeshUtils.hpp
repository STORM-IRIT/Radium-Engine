#ifndef RADIUMENGINE_MESHUTILS_HPP
#define RADIUMENGINE_MESHUTILS_HPP

#include <Core/RaCore.hpp>
#include <vector>
#include <array>

#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra
{
    namespace Core
    {
        /// Functions to operate on a TriangleMesh
        namespace MeshUtils
        {
            //
            // Geometry/Topology  utils
            //

            /// Fills the given array with the vertices of a given triangle.
            inline void getTriangleVertices( const TriangleMesh& mesh, TriangleIdx triIdx,
                                                    std::array<Vector3, 3>& verticesOut );

            /// Returns the area of a given triangle.
            inline Scalar getTriangleArea( const TriangleMesh& mesh, TriangleIdx triIdx );

            /// Computes the normal of a given triangle.
            inline Vector3 getTriangleNormal( const TriangleMesh& mesh, TriangleIdx triIdx );

            inline Aabb getAabb( const TriangleMesh& mesh );

            /// Automatically compute normals for each vertex by averaging connected triangle normals.
            RA_CORE_API void getAutoNormals( TriangleMesh& mesh, VectorArray<Vector3>& normalsOut );

            /// Finds the duplicate vertices in a mesh, returning an array indicating for each vertex where to find the
            /// first occurrence.
            RA_CORE_API bool findDuplicates( const TriangleMesh& mesh, std::vector<VertexIdx>& duplicatesMap );

            RA_CORE_API void removeDuplicates(TriangleMesh& mesh, std::vector<VertexIdx>& vertexMap);
            //
            // Primitive construction
            //

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

            //
            // Checks
            //

            /// Check that the mesh is well built, asserting when it is not.
            /// only compiles to something when in debug mode.
            RA_CORE_API void checkConsistency( const TriangleMesh& mesh );
        }
    }
}

#include <Core/Mesh/MeshUtils.inl>

#endif //RADIUMENGINE_MESHUTILS_HPP
