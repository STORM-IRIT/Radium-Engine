#ifndef RADIUMENGINE_MESHUTILS_HPP
#define RADIUMENGINE_MESHUTILS_HPP

#include <vector>
#include <array>

#include <array>

#include <mesh/TriangleMesh.hpp>

namespace Ra
{
    /// Functions to operate on a TriangleMesh
    namespace MeshUtils
    {
        //
        // Geometry/Topology  utils
        //

        /// Fills the given array with the vertices of a given triangle.
        inline void getTriangleVertices(const TriangleMesh& mesh, TriangleIdx triIdx,
                                        std::array<Vector3, 3>& verticesOut);

        /// Returns the area of a given triangle.
        inline float getTriangleArea(const TriangleMesh& mesh, TriangleIdx triIdx);

        /// Computes the normal of a given triangle.
        inline Vector3 getTriangleNormal(const TriangleMesh& mesh, TriangleIdx triIdx);

        inline Aabb getAabb(const TriangleMesh& mesh);

        /// Automatically compute normals for each vertex by averaging connected triangle normals.
        void getAutoNormals(TriangleMesh& mesh, VectorArray<Vector3>& normalsOut);

        /// Finds the duplicate vertices in a mesh, returning an array indicating for each vertex where to find the
        /// first occurrence.
        bool findDuplicates(const TriangleMesh& mesh, std::vector<VertexIdx>& duplicatesMap);

    }
}

#include <mesh/MeshUtils.inl>

#endif //RADIUMENGINE_MESHUTILS_HPP
