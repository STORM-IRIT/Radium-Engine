#include "MeshUtils.hpp"
#include <Core/Geometry/PointCloud/PointCloud.hpp>
namespace Ra
{
    namespace Core
    {
        namespace MeshUtils
        {
            inline Scalar getTriangleArea( const TriangleMesh& mesh, TriangleIdx triIdx )
            {
                std::array<Vector3, 3> v;
                getTriangleVertices( mesh, triIdx, v );

                const Vector3 edge0 = v[1] - v[0];
                const Vector3 edge1 = v[2] - v[0];

                return ( edge0.cross( edge1 ) ).norm() / 2.f;
            }

            inline Vector3 getTriangleNormal( const TriangleMesh& mesh, TriangleIdx triIdx )
            {
                std::array<Vector3, 3> v;
                getTriangleVertices( mesh, triIdx, v );

                const Vector3 edge0 = v[1] - v[0];
                const Vector3 edge1 = v[2] - v[0];
                return edge0.cross( edge1 ).normalized();
            }

            inline void getTriangleVertices( const TriangleMesh& mesh, TriangleIdx triIdx,
                                             std::array<Vector3, 3>& verticesOut )
            {
                const Triangle& tri = mesh.m_triangles[triIdx];
                for ( uint i = 0; i < 3; ++i )
                {
                    verticesOut[i] = mesh.m_vertices[tri[i]];
                }
            }

            inline Aabb getAabb( const TriangleMesh& mesh )
            {
                return PointCloud::aabb(mesh.m_vertices);
            }
        }
    }
}
