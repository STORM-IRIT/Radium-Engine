#include "MeshPrimitives.hpp"
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/String/StringUtils.hpp>
namespace Ra
{
    namespace Core
    {
        namespace MeshUtils
        {

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
        } // MeshUtils
    } // Core
}// Ra
