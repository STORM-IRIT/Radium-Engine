#include "MeshUtils.hpp"

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
                const Vector3 min = mesh.m_vertices.getMap().rowwise().minCoeff();
                const Vector3 max = mesh.m_vertices.getMap().rowwise().maxCoeff();
                return Aabb( min, max );
            }

            template< uint ROW, uint CLM >
            inline TriangleMesh makePlaneGrid( const Vector2& halfExts, const Transform& T ) {
                TriangleMesh grid;
                const uint R = ( ROW + 1 );
                const uint C = ( CLM + 1 );
                const uint v_size = C * R;
                const uint t_size = 2 * CLM * ROW;

                grid.m_vertices.resize( v_size );
                grid.m_normals.resize( v_size );
                grid.m_triangles.reserve( t_size );

                const Vector3 X = T.linear().col( 0 ).normalized();
                const Vector3 Y = T.linear().col( 1 ).normalized();
                const Vector3 Z = T.linear().col( 2 ).normalized();

                const Vector3 x = ( 2.0 * halfExts[0] * X ) / ( Scalar )( CLM );
                const Vector3 y = ( 2.0 * halfExts[1] * Y ) / ( Scalar )( ROW );
                const Vector3 o = T.translation() - ( halfExts[0] * X ) - ( halfExts[1] * Y );

                uint v[R][C];
                for( uint i = 0; i < R; ++i ) {
                    for( uint j = 0; j < C; ++j ) {
                        const uint id = ( i * C ) + j;
                        v[i][j] = id;
                        grid.m_vertices[id] = o + ( i * y ) + ( j * x );
                        grid.m_normals[id]  = Z;
                    }
                }

                for( uint i = 0; i < ROW; ++i ) {
                    for( uint j = 0; j < CLM; ++j ) {
                        //grid.m_triangles.push_back( Triangle( v[i][j],   v[i][j+1],   v[i+1][j] ) );
                        //grid.m_triangles.push_back( Triangle( v[i][j+1], v[i+1][j+1], v[i+1][j] ) );
                        grid.m_triangles.push_back( Triangle( v[i][j], v[i][j+1], v[i+1][j+1]  ) );
                        grid.m_triangles.push_back( Triangle( v[i][j], v[i+1][j+1], v[i+1][j]  ) );
                    }
                }

                return grid;
            }

            template<uint U, uint V>
            TriangleMesh makeParametricSphere( Scalar radius )
            {
                const uint slices = U;
                const uint stacks = V;
                TriangleMesh result;
                for ( uint u = 0; u < slices; ++u )
                {
                    const Scalar theta = Scalar( 2 * u ) * Core::Math::Pi / Scalar( slices );
                    for ( uint v = 1; v < stacks; ++v )
                    {
                        // Regular vertices on the sphere.
                        const Scalar phi = Scalar( v ) * Core::Math::Pi / Scalar( stacks );
                        result.m_vertices.push_back( Vector3(
                                                         radius * std::cos( theta ) * std::sin( phi ),
                                                         radius * std::sin( theta ) * std::sin( phi ),
                                                         radius * std::cos( phi )
                                                     ) );
                        // Regular triangles
                        if ( v > 1 && v < stacks )
                        {
                            const uint nextSlice = ( ( u + 1 ) % slices ) * ( stacks - 1 );
                            const uint baseSlice = u * ( stacks - 1 );
                            result.m_triangles.push_back(
                                Triangle( baseSlice + v - 2, baseSlice + v - 1, nextSlice + v - 1 ) );
                            result.m_triangles.push_back(
                                Triangle( baseSlice + v - 2, nextSlice + v - 1, nextSlice + v - 2 ) );
                        }
                    }
                }

                // Add the pole vertices.
                uint northPoleIdx = result.m_vertices.size();
                result.m_vertices.push_back( Vector3( 0, 0, radius ) );
                uint southPoleIdx = result.m_vertices.size();
                result.m_vertices.push_back( Vector3( 0, 0, -radius ) );

                // Add the polar caps triangles.
                for ( uint u = 0; u < slices; ++u )
                {
                    const uint nextSlice = ( ( u + 1 ) % slices ) * ( stacks - 1 );
                    const uint baseSlice = u * ( stacks - 1 );
                    result.m_triangles.push_back( Triangle( northPoleIdx, baseSlice, nextSlice ) );
                    result.m_triangles.push_back(
                        Triangle( southPoleIdx, nextSlice + stacks - 2, baseSlice + stacks - 2 ) );
                }

                // Compute normals.
                for ( const auto v : result.m_vertices )
                {
                    const Vector3 n = v.normalized();
                    result.m_normals.push_back( n );
                }
                checkConsistency( result );
                return result;
            }


            template<uint U, uint V>
            TriangleMesh makeParametricTorus(Scalar majorRadius, Scalar minorRadius)
            {
                TriangleMesh result;

                for (uint iu = 0; iu < U; ++iu)
                {
                    Scalar u = Scalar(iu) * Core::Math::PiMul2 / Scalar(U);
                    Core::Vector3 circleCenter(majorRadius * std::cos(u), majorRadius * std::sin(u), 0.f);

                    for (uint iv = 0; iv < V; ++iv)
                    {
                        Scalar v = Scalar(iv) * Core::Math::PiMul2 / Scalar(V);

                        Core::Vector3 vertex (
                                (majorRadius + minorRadius * std::cos(v)) * std::cos(u),
                                (majorRadius + minorRadius * std::cos(v)) * std::sin(u),
                                 minorRadius * std::sin(v));


                        result.m_vertices.push_back(vertex);
                        result.m_normals.push_back((vertex - circleCenter).normalized());

                        result.m_triangles.push_back(Triangle(iu*V + iv, ((iu + 1) % U)*V + iv, iu * V + ((iv + 1) % V)));
                        result.m_triangles.push_back(Triangle(iu * V + iv, iu * V + ((iv + 1) % V), ((iu - 1) % U) * V + ((iv + 1) % V)));
                    }
                }
                checkConsistency(result);
                return result;
            }
        }
    }
}
