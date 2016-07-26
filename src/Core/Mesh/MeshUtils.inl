#include "MeshUtils.hpp"
#include <Core/Geometry/PointCloud/PointCloud.hpp>
#include <Core/Geometry/Triangle/TriangleOperation.hpp>

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

                return Geometry::triangleArea( v[0], v[1], v[2] );
            }

            inline Vector3 getTriangleNormal( const TriangleMesh& mesh, TriangleIdx triIdx )
            {
                std::array<Vector3, 3> v;
                getTriangleVertices( mesh, triIdx, v );
                return Geometry::triangleNormal( v[0], v[1], v[2] );
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
