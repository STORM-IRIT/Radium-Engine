#include "TriangleMesh.hpp"

namespace Ra
{
    namespace Core
    {
        inline void TriangleMesh::clear()
        {
            m_vertices.clear();
            m_normals.clear();
            m_triangles.clear();
            m_faces.clear();
        }

        inline void TriangleMesh::append( const TriangleMesh& other )
        {
            CORE_WARN_IF( other.m_triangles.empty() && !other.m_faces.empty(),
                          "TriangleMesh to append is not correct" );

            const std::size_t verticesBefore = m_vertices.size();
            const std::size_t trianglesBefore = m_triangles.size();
            const std::size_t facesBefore = m_faces.size();

            m_vertices.insert( m_vertices.end(), other.m_vertices.cbegin(), other.m_vertices.cend() );
            m_normals.insert( m_normals.end(), other.m_normals.cbegin(), other.m_normals.cend() );
            m_triangles.insert( m_triangles.end(), other.m_triangles.cbegin(), other.m_triangles.cend() );
            m_faces.insert( m_faces.end(), other.m_faces.cbegin(), other.m_faces.cend() );

            // Offset the vertex indices in the faces
            for ( uint t = trianglesBefore; t < m_triangles.size(); ++t )
            {
                for ( uint i = 0; i < 3; ++i )
                {
                    m_triangles[t][i] += verticesBefore;
                }
            }
            for ( uint t = facesBefore; t < m_faces.size(); ++t )
            {
                for ( uint i = 0; i < 4; ++i )
                {
                    m_faces[t][i] += verticesBefore;
                }
            }
        }

        inline void TriangleMesh::triangulate()
        {
            m_triangles.clear();
            for (const VectorNui face : m_faces)
            {
                // just register triangle as is
                if (face.size() == 3)
                {
                    m_triangles.push_back( face.head<3>() );
                    continue;
                }
                // don't deal with n-gons (n>4) for now
                if (face.size() > 4)
                {
                    continue;
                }
                // split quad in 2 triangles
                Triangle t1;
                Triangle t2;
                // check convexity
                const Vector3& p0 = m_vertices[ face[0] ];
                const Vector3& p1 = m_vertices[ face[1] ];
                const Vector3& p2 = m_vertices[ face[2] ];
                const Vector3& p3 = m_vertices[ face[3] ];
                const Vector3 n1 = (p1-p0).cross(p2-p0);
                const Vector3 n2 = (p2-p0).cross(p3-p0);
                if (n1.dot(n2) > 0)
                {
                    t1 << face[0], face[1], face[2];
                    t2 << face[0], face[2], face[3];
                }
                else
                {
                    t1 << face[0], face[1], face[3];
                    t2 << face[1], face[2], face[3];
                }
                m_triangles.push_back( t1 );
                m_triangles.push_back( t2 );
            }
        }
    }
}
