#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>

#include <Core/TreeStructures/kdtree.hpp>

namespace Ra
{
    namespace Core
    {
        /// A very basic structure representing a triangle mesh which stores the bare minimum :
        /// vertices, faces and normals. See MeshUtils for geometric functions operating on a mesh.
        struct TriangleMesh
        {
            /// Create an empty mesh.
            inline TriangleMesh() {}
            /// Copy constructor and assignment operator
            TriangleMesh( const TriangleMesh& ) = default;
            TriangleMesh& operator= ( const TriangleMesh& ) = default;

            /// Erases all data, making the mesh empty.
            inline void clear();

            /// Appends another mesh to this one.
            inline void append( const TriangleMesh& other );

            //inline void computeKdTree();

            VectorArray<Vector3>  m_vertices;
            VectorArray<Vector3>  m_normals;
            VectorArray<Triangle> m_triangles;

            //Super4PCS::KdTree<float>* m_kdtree;

        };
    }
}

#include <Core/Mesh/TriangleMesh.inl>

#endif //RADIUMENGINE_TRIANGLEMESH_HPP
