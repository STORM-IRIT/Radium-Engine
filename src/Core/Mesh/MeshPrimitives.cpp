#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Containers/Grid.hpp>

namespace Ra
{
    namespace Core
    {
        namespace MeshUtils
        {

            TriangleMesh makeXNormalQuad( const Vector2& halfExts )
            {
                Transform T = Transform::Identity();
                T.linear().col( 0 ).swap( T.linear().col( 1 ) );
                T.linear().col( 1 ).swap( T.linear().col( 2 ) );
                return makePlaneGrid( 1, 1, halfExts, T );
            }

            TriangleMesh makeYNormalQuad( const Vector2& halfExts )
            {
                Transform T = Transform::Identity();
                T.linear().col( 1 ).swap( T.linear().col( 2 ) );
                T.linear().col( 0 ).swap( T.linear().col( 1 ) );
                return makePlaneGrid( 1, 1, halfExts, T );
            }

            TriangleMesh makeZNormalQuad( const Vector2& halfExts )
            {
                return makePlaneGrid( 1, 1, halfExts );
            }

            TriangleMesh makeBox( const Vector3& halfExts )
            {
                Aabb aabb( -halfExts, halfExts );
                return makeBox( aabb );
            }

            TriangleMesh makeBox( const Aabb& aabb )
            {
                TriangleMesh result;
                result.m_vertices =
                {
                    aabb.corner( Aabb::BottomLeftFloor ),
                    aabb.corner( Aabb::BottomRightFloor ),
                    aabb.corner( Aabb::TopLeftFloor ),
                    aabb.corner( Aabb::TopRightFloor ),
                    aabb.corner( Aabb::BottomLeftCeil ),
                    aabb.corner( Aabb::BottomRightCeil ),
                    aabb.corner( Aabb::TopLeftCeil ),
                    aabb.corner( Aabb::TopRightCeil )
                };
                result.m_triangles =
                {
                    Triangle( 0, 2, 1 ), Triangle( 1, 2, 3 ),   // Floor
                    Triangle( 0, 1, 4 ), Triangle( 4, 1, 5 ),   // Front
                    Triangle( 3, 2, 6 ), Triangle( 3, 6, 7 ),   // Back
                    Triangle( 5, 1, 3 ), Triangle( 5, 3, 7 ),   // Right
                    Triangle( 2, 0, 4 ), Triangle( 2, 4, 6 ),   // Left
                    Triangle( 4, 5, 6 ), Triangle( 6, 5, 7 )    // Top
                };

                getAutoNormals( result, result.m_normals );
                checkConsistency( result );
                return result;

            }

            TriangleMesh makeGeodesicSphere( Scalar radius, uint numSubdiv )
            {
                TriangleMesh result;
                // First, make an icosahedron.
                // Top vertex
                result.m_vertices.push_back( Vector3( 0, 0, radius ) );

                const Scalar sq5_5 = radius * std::sqrt( 5.f ) / 5.f;

                // Middle vertices are on pentagons inscribed on a circle of radius 2*sqrt(5)
                for ( int i = 0; i < 5 ; ++i )
                {
                    for ( int j = 0; j < 2 ; ++j )
                    {
                        const Scalar theta = ( Scalar( i ) + ( j * 0.5f ) ) * Math::PiMul2 / 5.f;

                        const Scalar x = 2.f * sq5_5 * std::cos( theta );
                        const Scalar y = 2.f * sq5_5 * std::sin( theta );
                        const Scalar z = j == 0 ? sq5_5 : -sq5_5;
                        result.m_vertices.push_back( Vector3( x, y, z ) );
                    }
                }

                // Bottom vertex
                result.m_vertices.push_back( Vector3( 0, 0, -radius ) );

                for ( int i = 0; i < 5; ++i )
                {
                    uint i1 = ( i + 1 ) % 5;
                    // Top triangles
                    result.m_triangles.push_back( Triangle( 0, 2 * i + 1, ( 2 * i1 + 1 ) ) );

                    // Bottom triangles
                    result.m_triangles.push_back( Triangle( 2 * i + 2, 11, ( 2 * i1 + 2 ) ) );

                }
                for ( uint i = 0; i < 10; ++i )
                {
                    uint i1 = ( i + 0 ) % 10 + 1;
                    uint i2 = ( i + 1 ) % 10 + 1;
                    uint i3 = ( i + 2 ) % 10 + 1;
                    i % 2 ?  result.m_triangles.push_back( Triangle( i3, i2, i1 ) )
                    : result.m_triangles.push_back( Triangle( i2, i3, i1 ) );
                }


                for ( uint n = 0; n < numSubdiv; ++n )
                {
                    VectorArray<Triangle> newTris; //= result.m_triangles;
                    // Now subdivide every face into 4 triangles.
                    for ( uint i = 0 ; i < result.m_triangles.size(); ++i )
                    {
                        const Triangle& tri = result.m_triangles[i];
                        std::array<Vector3, 3> triVertices;
                        std::array<uint, 3> middles;

                        getTriangleVertices( result, i, triVertices );

                        for ( int v = 0; v < 3 ; ++v )
                        {
                            middles[v] = result.m_vertices.size();
                            result.m_vertices.push_back( 0.5f * ( triVertices[v] + triVertices[( v + 1 ) % 3] ) );
                        }

                        newTris.push_back( Triangle( tri[0], middles[0], middles[2] ) );
                        newTris.push_back( Triangle( middles[0], tri[1], middles[1] ) );
                        newTris.push_back( Triangle( middles[2], middles[1], tri[2] ) );
                        newTris.push_back( Triangle( middles[0], middles[1], middles[2] ) );

                    }
                    result.m_triangles = newTris;
                }

                // Project vertices on the sphere
                for ( auto& vertex : result.m_vertices )
                {
                    const Scalar r = radius / vertex.norm();
                    vertex *= r;
                }
                getAutoNormals( result, result.m_normals );
                checkConsistency( result );
                return result;
            }

            TriangleMesh makeCylinder(const Vector3& a, const Vector3& b, Scalar radius, uint nFaces)
            {
                TriangleMesh result;

                Core::Vector3 ab = b - a;

                //  Create two circles normal centered on A and B and normal to ab;
                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(ab, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                Core::Vector3 c = 0.5 * (a + b);

                result.m_vertices.push_back(a);
                result.m_vertices.push_back(b);

                const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
                for (uint i = 0; i < nFaces; ++i)
                {
                    const Scalar theta = i * thetaInc;
                    // Even indices are A circle and odd indices are B circle.
                    result.m_vertices.push_back(a + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane ));
                    result.m_vertices.push_back(c + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane ));
                    result.m_vertices.push_back(b + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane ));
                }


                for (uint i = 0; i < nFaces; ++i)
                {
                    uint bl = 3*i +2 ; // bottom left corner of face
                    uint br = 3*((i+1)%nFaces) +2 ; // bottom right corner of face
                    uint ml = bl +1; // mid left
                    uint mr = br +1; // mid right
                    uint tl = ml +1; // top left
                    uint tr = mr +1; // top right

                    result.m_triangles.push_back(Triangle( bl, br, ml ));
                    result.m_triangles.push_back(Triangle( br, mr, ml ));

                    result.m_triangles.push_back(Triangle( ml, mr, tl ));
                    result.m_triangles.push_back(Triangle( mr, tr, tl ));

                    result.m_triangles.push_back(Triangle(0, br, bl));
                    result.m_triangles.push_back(Triangle(1, tl, tr));
                }
                getAutoNormals(result,result.m_normals);
                checkConsistency( result );
                return result;
            }

            TriangleMesh makeTube(const Vector3& a, const Vector3& b, Scalar outerRadius, Scalar innerRadius, uint nFaces)
            {

                CORE_ASSERT(outerRadius > innerRadius, "Outer radius must be bigger than inner.");

                TriangleMesh result;

                Core::Vector3 ab = b - a;

                //  Create two circles normal centered on A and B and normal to ab;
                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(ab, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                Core::Vector3 c = 0.5 * (a + b);

                const Scalar thetaInc(Core::Math::PiMul2 / Scalar(nFaces));
                for (uint i = 0; i < nFaces; ++i)
                {
                    const Scalar theta = i * thetaInc;
                    result.m_vertices.push_back(
                            a + outerRadius * (std::cos(theta) * xPlane + std::sin(theta) * yPlane));
                    result.m_vertices.push_back(
                            c + outerRadius * (std::cos(theta) * xPlane + std::sin(theta) * yPlane));
                    result.m_vertices.push_back(
                            b + outerRadius * (std::cos(theta) * xPlane + std::sin(theta) * yPlane));

                    result.m_vertices.push_back(
                            a + innerRadius * (std::cos(theta) * xPlane + std::sin(theta) * yPlane));
                    result.m_vertices.push_back(
                            c + innerRadius * (std::cos(theta) * xPlane + std::sin(theta) * yPlane));
                    result.m_vertices.push_back(
                            b + innerRadius * (std::cos(theta) * xPlane + std::sin(theta) * yPlane));
                }


                for (uint i = 0; i < nFaces; ++i)
                {
                    // Outer face.
                    uint obl = 6 * i; // bottom left corner of outer face
                    uint obr = 6 * ((i + 1) % nFaces); // bottom right corner of outer face
                    uint oml = obl + 1; // mid left
                    uint omr = obr + 1; // mid right
                    uint otl = oml + 1; // top left
                    uint otr = omr + 1; // top right

                    // Inner face
                    uint ibl = 6 * i + 3; // bottom left corner of inner face
                    uint ibr = 6 * ((i + 1) % nFaces) + 3; // bottom right corner of inner face
                    uint iml = ibl + 1; // mid left
                    uint imr = ibr + 1; // mid right
                    uint itl = iml + 1; // top left
                    uint itr = imr + 1; // top right

                    // Outer face triangles, just like a regular cylinder.

                    result.m_triangles.push_back(Triangle(obl, obr, oml));
                    result.m_triangles.push_back(Triangle(obr, omr, oml));

                    result.m_triangles.push_back(Triangle(oml, omr, otl));
                    result.m_triangles.push_back(Triangle(omr, otr, otl));

                    // Inner face triangles (note how order is reversed because inner face points inwards).

                    result.m_triangles.push_back(Triangle(ibr, ibl, iml));
                    result.m_triangles.push_back(Triangle(ibr, iml, imr));

                    result.m_triangles.push_back(Triangle(imr, iml, itl));
                    result.m_triangles.push_back(Triangle(imr, itl, itr));

                    // Bottom face quad
                    result.m_triangles.push_back(Triangle(ibr, obr, ibl));
                    result.m_triangles.push_back(Triangle(obl, ibl, obr));

                    // Top face quad
                    result.m_triangles.push_back(Triangle(otr, itr, itl));
                    result.m_triangles.push_back(Triangle(itl, otl, otr));

                }
                getAutoNormals(result, result.m_normals);
                checkConsistency(result);
                return result;
            }


            TriangleMesh makeCone(const Vector3& base, const Vector3& tip, Scalar radius, uint nFaces)
            {
                TriangleMesh result;

                Core::Vector3 ab = tip - base;

                //  Create two circles normal centered on A and B and normal to ab;
                Core::Vector3 xPlane, yPlane;
                Core::Vector::getOrthogonalVectors(ab, xPlane, yPlane);
                xPlane.normalize();
                yPlane.normalize();

                result.m_vertices.push_back(base);
                result.m_vertices.push_back(tip);

                const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
                for (uint i = 0; i < nFaces; ++i)
                {
                    const Scalar theta =  i*thetaInc;
                    result.m_vertices.push_back(base + radius* ( std::cos (theta) * xPlane + std::sin (theta) * yPlane ));
                }

                for (uint i = 0; i < nFaces; ++i)
                {
                    uint bl = i + 2; // bottom left corner of face
                    uint br = ((i+1)%nFaces) + 2; // bottom right corner of face

                    result.m_triangles.push_back(Triangle( 0, br, bl ));
                    result.m_triangles.push_back(Triangle( 1, bl, br));
                }
                getAutoNormals(result,result.m_normals);
                checkConsistency( result );
                return result;
            }

            void checkConsistency( const TriangleMesh& mesh )
            {
#ifdef CORE_DEBUG
                std::vector<bool> visited( mesh.m_vertices.size(), false );
                for ( uint t = 0 ; t < mesh.m_triangles.size(); ++t )
                {
                    std::string errStr;
                    StringUtils::stringPrintf( errStr, "Triangle %d is degenerate", t);
                    CORE_WARN_IF(!(getTriangleArea(mesh,t) > 0.f) , errStr.c_str());
                    const Triangle& tri = mesh.m_triangles[t];
                    for ( uint i = 0; i < 3; ++i )
                    {
                        StringUtils::stringPrintf( errStr, "Vertex %d is in triangle %d (#%d) is out of bounds", tri[i], t, i );
                        CORE_ASSERT( uint( tri[i] ) < mesh.m_vertices.size(), errStr.c_str() );
                        visited[tri[i]] = true;
                    }
                }

                for ( uint v = 0; v < visited.size(); ++v )
                {
                    std::string errStr;
                    StringUtils::stringPrintf( errStr, "Vertex %d does not belong to any triangle", v );
                    CORE_ASSERT( visited[v], errStr.c_str() );
                }

                // Normals are optional but if they are present then every vertex should have one.
                CORE_ASSERT( mesh.m_normals.size() ==  0 || mesh.m_normals.size() == mesh.m_vertices.size(),
                             "Inconsistent number of normals" );
#endif
            }

            TriangleMesh makePlaneGrid( const uint rows, const uint cols, const Vector2& halfExts, const Transform& T )
            {
                TriangleMesh grid;
                const uint R = ( rows + 1 );
                const uint C = ( cols + 1 );
                const uint v_size = C * R;
                const uint t_size = 2 * cols * rows;

                grid.m_vertices.resize( v_size );
                grid.m_normals.resize( v_size );
                grid.m_triangles.reserve( t_size );

                const Vector3 X = T.linear().col( 0 ).normalized();
                const Vector3 Y = T.linear().col( 1 ).normalized();
                const Vector3 Z = T.linear().col( 2 ).normalized();

                const Vector3 x = ( 2.0 * halfExts[0] * X ) / ( Scalar )( cols );
                const Vector3 y = ( 2.0 * halfExts[1] * Y ) / ( Scalar )( rows );
                const Vector3 o = T.translation() - ( halfExts[0] * X ) - ( halfExts[1] * Y );

                Grid < uint, 2> v( { R,C } );
                for( uint i = 0; i < R; ++i )
                {
                    for( uint j = 0; j < C; ++j )
                    {
                        const uint id = ( i * C ) + j;
                        v.at({ i,j }) = id;
                        grid.m_vertices[id] = o + ( i * y ) + ( j * x );
                        grid.m_normals[id]  = Z;
                    }
                }

                for( uint i = 0; i < rows; ++i )
                {
                    for( uint j = 0; j < cols; ++j )
                    {
                        grid.m_triangles.push_back( Triangle( v.at( { i,j } ), v.at( { i,j+1 } ), v.at( { i+1,j+1 } )));
                        grid.m_triangles.push_back( Triangle( v.at( { i,j } ), v.at( { i+1,j+1 } ), v.at( { i+1,j } )));
                    }
                }

                return grid;
            }
        } // MeshUtils
    } // Core
}// Ra
