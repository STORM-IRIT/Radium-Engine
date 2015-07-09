#ifndef RADIUMENGINE_TRIANGLE_MESH_H
#define RADIUMENGINE_TRIANGLE_MESH_H

#include <math/Vector.hpp>
#include <containers/VectorArray.hpp>
#include <mesh/MeshTypes.hpp>

namespace Ra{
    /// A very basic structure representing a triangle mesh which stores the bare minimum :
    /// vertices, faces and normals. See MeshUtils for geometric functions operating on a mesh.
    struct TriangleMesh
    {
        /// Create an empty mesh.
        TriangleMesh() {}
        /// Copy constructor and assignment operator
        TriangleMesh( const TriangleMesh& ) = default;
        TriangleMesh& operator= ( const TriangleMesh& ) = default;

        /// Erases all data, making the mesh empty.
        inline void clear();

        /// Appends another mesh to this one.
        inline void append( const TriangleMesh& other);

        VectorArray<Vector3>  m_vertices;
        VectorArray<Vector3>  m_normals;
        VectorArray<Triangle> m_triangles;
    };


}

#include <mesh/TriangleMesh.inl>
#endif //RADIUMENGINE_TRIANGLE_MESH_H
