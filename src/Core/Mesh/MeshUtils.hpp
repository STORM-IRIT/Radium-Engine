#ifndef RADIUMENGINE_MESHUTILS_HPP
#define RADIUMENGINE_MESHUTILS_HPP

#include <Core/RaCore.hpp>
#include <vector>
#include <array>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Math/Ray.hpp>

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
            
            /// If t1 is triangle (v1,v2,v3), returns v3.
            inline uint getLastVertex( const Triangle& t1, uint v1, uint v2 );
            
            /// Returns true if the triangle contains a given edge
            inline bool containsEdge( const Triangle& t1, uint v1, uint v2);
            
            /// Automatically compute normals for each vertex by averaging connected triangle normals.
            RA_CORE_API void getAutoNormals( TriangleMesh& mesh, VectorArray<Vector3>& normalsOut );
            
            /// Finds the duplicate vertices in a mesh, returning an array indicating for each vertex where to find the
            /// first occurrence.
            RA_CORE_API bool findDuplicates( const TriangleMesh& mesh, std::vector<VertexIdx>& duplicatesMap );
            
            RA_CORE_API void removeDuplicates(TriangleMesh& mesh, std::vector<VertexIdx>& vertexMap);
            
            
            /// Returns a list of edges from a given triangle mesh
            RA_CORE_API inline std::vector<Ra::Core::Vector2ui> getEdges( const TriangleMesh& mesh );
            
            /// Results of a raycast vs a mesh
            struct RayCastResult
            {
                int m_hitTriangle   = -1;
                int m_nearestVertex = -1;
                int m_edgeVertex0   = -1;
                int m_edgeVertex1   = -1;
                Scalar m_t          = -1;
            };
            
            /// Return the index of the triangle hit by the ray or -1 if there's no hit.
            RA_CORE_API RayCastResult castRay( const TriangleMesh& mesh, const Ray& ray);
            
            /// Return the mean edge length of the given triangle mesh
            RA_CORE_API Scalar getMeanEdgeLength( const TriangleMesh& mesh );
            
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
