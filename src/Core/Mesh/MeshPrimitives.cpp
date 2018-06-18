#include <Core/Containers/Grid.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>

namespace Ra {
namespace Core {
namespace MeshUtils {

TriangleMesh makeXNormalQuad( const Vector2& halfExts ) {
    Transform T = Transform::Identity();
    T.linear().col( 0 ).swap( T.linear().col( 1 ) );
    T.linear().col( 1 ).swap( T.linear().col( 2 ) );
    return makePlaneGrid( 1, 1, halfExts, T );
}

TriangleMesh makeYNormalQuad( const Vector2& halfExts ) {
    Transform T = Transform::Identity();
    T.linear().col( 1 ).swap( T.linear().col( 2 ) );
    T.linear().col( 0 ).swap( T.linear().col( 1 ) );
    return makePlaneGrid( 1, 1, halfExts, T );
}

TriangleMesh makeZNormalQuad( const Vector2& halfExts ) {
    return makePlaneGrid( 1, 1, halfExts );
}

TriangleMesh makeBox( const Vector3& halfExts ) {
    Aabb aabb( -halfExts, halfExts );
    return makeBox( aabb );
}

TriangleMesh makeBox( const Aabb& aabb ) {
    TriangleMesh result;
    result.vertices() = {
        aabb.corner( Aabb::BottomLeftFloor ), aabb.corner( Aabb::BottomRightFloor ),
        aabb.corner( Aabb::TopLeftFloor ),    aabb.corner( Aabb::TopRightFloor ),
        aabb.corner( Aabb::BottomLeftCeil ),  aabb.corner( Aabb::BottomRightCeil ),
        aabb.corner( Aabb::TopLeftCeil ),     aabb.corner( Aabb::TopRightCeil )};

    Scalar a = 1. / std::sqrt( 3. );
    result.normals() = {Vector3( -a, -a, -a ), Vector3( +a, -a, -a ), Vector3( -a, +a, -a ),
                        Vector3( +a, +a, -a ), Vector3( -a, -a, +a ), Vector3( +a, -a, +a ),
                        Vector3( -a, +a, +a ), Vector3( +a, +a, +a )};

    result.m_triangles = {
        Triangle( 0, 2, 1 ), Triangle( 1, 2, 3 ), // Floor
        Triangle( 0, 1, 4 ), Triangle( 4, 1, 5 ), // Front
        Triangle( 3, 2, 6 ), Triangle( 3, 6, 7 ), // Back
        Triangle( 5, 1, 3 ), Triangle( 5, 3, 7 ), // Right
        Triangle( 2, 0, 4 ), Triangle( 2, 4, 6 ), // Left
        Triangle( 4, 5, 6 ), Triangle( 6, 5, 7 )  // Top
    };

    checkConsistency( result );
    return result;
}

TriangleMesh makeSharpBox( const Vector3& halfExts ) {
    Aabb aabb( -halfExts, halfExts );
    return makeSharpBox( aabb );
}

TriangleMesh makeSharpBox( const Aabb& aabb ) {
    TriangleMesh result;
    result.vertices() = {// Floor Face
                         aabb.corner( Aabb::BottomLeftFloor ), aabb.corner( Aabb::TopLeftFloor ),
                         aabb.corner( Aabb::TopRightFloor ), aabb.corner( Aabb::BottomRightFloor ),

                         // Ceil Face
                         aabb.corner( Aabb::BottomLeftCeil ), aabb.corner( Aabb::BottomRightCeil ),
                         aabb.corner( Aabb::TopRightCeil ), aabb.corner( Aabb::TopLeftCeil ),

                         // Left Face
                         aabb.corner( Aabb::TopLeftFloor ), aabb.corner( Aabb::BottomLeftFloor ),
                         aabb.corner( Aabb::BottomLeftCeil ), aabb.corner( Aabb::TopLeftCeil ),

                         // Right Face
                         aabb.corner( Aabb::BottomRightFloor ), aabb.corner( Aabb::TopRightFloor ),
                         aabb.corner( Aabb::TopRightCeil ), aabb.corner( Aabb::BottomRightCeil ),

                         // Bottom Face
                         aabb.corner( Aabb::BottomLeftFloor ),
                         aabb.corner( Aabb::BottomRightFloor ),
                         aabb.corner( Aabb::BottomRightCeil ), aabb.corner( Aabb::BottomLeftCeil ),

                         // Top face
                         aabb.corner( Aabb::TopLeftFloor ), aabb.corner( Aabb::TopLeftCeil ),
                         aabb.corner( Aabb::TopRightCeil ), aabb.corner( Aabb::TopRightFloor )};

    result.normals() = {
        // Floor face
        Vector3( 0, 0, -1 ), Vector3( 0, 0, -1 ), Vector3( 0, 0, -1 ), Vector3( 0, 0, -1 ),
        // Ceil Face
        Vector3( 0, 0, +1 ), Vector3( 0, 0, +1 ), Vector3( 0, 0, +1 ), Vector3( 0, 0, +1 ),
        // Left Face
        Vector3( -1, 0, 0 ), Vector3( -1, 0, 0 ), Vector3( -1, 0, 0 ), Vector3( -1, 0, 0 ),
        // Right Face
        Vector3( +1, 0, 0 ), Vector3( +1, 0, 0 ), Vector3( +1, 0, 0 ), Vector3( +1, 0, 0 ),
        // Bottom Face
        Vector3( 0, -1, 0 ), Vector3( 0, -1, 0 ), Vector3( 0, -1, 0 ), Vector3( 0, -1, 0 ),
        // Top Face
        Vector3( 0, +1, 0 ), Vector3( 0, +1, 0 ), Vector3( 0, +1, 0 ), Vector3( 0, +1, 0 )};

    result.m_triangles = {

        Triangle( 0, 1, 2 ),    Triangle( 0, 2, 3 ),    // Floor
        Triangle( 4, 5, 6 ),    Triangle( 4, 6, 7 ),    // Ceil
        Triangle( 8, 9, 10 ),   Triangle( 8, 10, 11 ),  // Left
        Triangle( 12, 13, 14 ), Triangle( 12, 14, 15 ), // Right
        Triangle( 16, 17, 18 ), Triangle( 16, 18, 19 ), // Bottom
        Triangle( 20, 21, 22 ), Triangle( 20, 22, 23 )  // Top
    };

    checkConsistency( result );
    return result;
}

TriangleMesh makeGeodesicSphere( Scalar radius, uint numSubdiv ) {
    TriangleMesh result;
    uint faceCount = std::pow( 4, numSubdiv ) * 20;
    result.vertices().reserve( faceCount - 8 );
    result.normals().reserve( faceCount - 8 );
    result.m_triangles.reserve( faceCount );

    // First, make an icosahedron.
    // Top vertex
    result.vertices().emplace_back( 0, 0, radius );
    result.normals().emplace_back( 0, 0, 1 );

    const Scalar sq5_5 = radius * std::sqrt( 5.f ) / 5.f;

    // Middle vertices are on pentagons inscribed on a circle of radius 2*sqrt(5)
    for ( int i = 0; i < 5; ++i )
    {
        for ( int j = 0; j < 2; ++j )
        {
            const Scalar theta = ( Scalar( i ) + ( j * 0.5f ) ) * Math::PiMul2 / 5.f;

            const Scalar x = 2.f * sq5_5 * std::cos( theta );
            const Scalar y = 2.f * sq5_5 * std::sin( theta );
            const Scalar z = j == 0 ? sq5_5 : -sq5_5;
            result.vertices().emplace_back( x, y, z );
            result.normals().push_back( result.vertices().back().normalized() );
        }
    }

    // Bottom vertex
    result.vertices().emplace_back( 0, 0, -radius );
    result.normals().emplace_back( 0, 0, -1 );

    for ( int i = 0; i < 5; ++i )
    {
        uint i1 = ( i + 1 ) % 5;
        // Top triangles
        result.m_triangles.emplace_back( 0, 2 * i + 1, ( 2 * i1 + 1 ) );

        // Bottom triangles
        result.m_triangles.emplace_back( 2 * i + 2, 11, ( 2 * i1 + 2 ) );
    }
    for ( uint i = 0; i < 10; ++i )
    {
        uint i1 = ( i + 0 ) % 10 + 1;
        uint i2 = ( i + 1 ) % 10 + 1;
        uint i3 = ( i + 2 ) % 10 + 1;
        i % 2 ? result.m_triangles.emplace_back( i3, i2, i1 )
              : result.m_triangles.emplace_back( i2, i3, i1 );
    }

    for ( uint n = 0; n < numSubdiv; ++n )
    {
        VectorArray<Triangle> newTris; //= result.m_triangles;
        // Now subdivide every face into 4 triangles.
        for ( uint i = 0; i < result.m_triangles.size(); ++i )
        {
            const Triangle& tri = result.m_triangles[i];
            std::array<Vector3, 3> triVertices;
            std::array<uint, 3> middles;

            getTriangleVertices( result, i, triVertices );

            for ( int v = 0; v < 3; ++v )
            {
                middles[v] = result.vertices().size();

                Vector3 vertex = 0.5f * ( triVertices[v] + triVertices[( v + 1 ) % 3] );
                vertex.normalize();

                result.vertices().push_back( radius * vertex );
                result.normals().push_back( vertex );
            }

            newTris.emplace_back( tri[0], middles[0], middles[2] );
            newTris.emplace_back( middles[0], tri[1], middles[1] );
            newTris.emplace_back( middles[2], middles[1], tri[2] );
            newTris.emplace_back( middles[0], middles[1], middles[2] );
        }
        result.m_triangles = newTris;
    }

    checkConsistency( result );
    return result;
}

TriangleMesh makeCylinder( const Vector3& a, const Vector3& b, Scalar radius, uint nFaces ) {
    TriangleMesh result;
    result.vertices().reserve( 2 + 3 * nFaces );
    result.normals().reserve( 2 + 3 * nFaces );
    result.m_triangles.reserve( 6 * nFaces );

    Vector3 ab = b - a;
    Vector3 dir = ab.normalized();

    //  Create two circles normal centered on A and B and normal to ab;
    Vector3 xPlane, yPlane;
    Vector::getOrthogonalVectors( ab, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    Vector3 c = 0.5 * ( a + b );

    result.vertices().push_back( a );
    result.vertices().push_back( b );
    result.normals().push_back( -dir );
    result.normals().push_back( dir );

    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i )
    {
        const Scalar theta = i * thetaInc;
        Vector3 normal = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

        // Even indices are A circle and odd indices are B circle.
        result.vertices().push_back( a + radius * normal );
        result.vertices().push_back( c + radius * normal );
        result.vertices().push_back( b + radius * normal );

        result.normals().push_back( ( normal - dir ).normalized() );
        result.normals().push_back( normal.normalized() );
        result.normals().push_back( ( normal + dir ).normalized() );
    }

    for ( uint i = 0; i < nFaces; ++i )
    {
        uint bl = 3 * i + 2;                      // bottom left corner of face
        uint br = 3 * ( ( i + 1 ) % nFaces ) + 2; // bottom right corner of face
        uint ml = bl + 1;                         // mid left
        uint mr = br + 1;                         // mid right
        uint tl = ml + 1;                         // top left
        uint tr = mr + 1;                         // top right

        result.m_triangles.emplace_back( bl, br, ml );
        result.m_triangles.emplace_back( br, mr, ml );

        result.m_triangles.emplace_back( ml, mr, tl );
        result.m_triangles.emplace_back( mr, tr, tl );

        result.m_triangles.emplace_back( 0, br, bl );
        result.m_triangles.emplace_back( 1, tl, tr );
    }
    checkConsistency( result );
    return result;
}

TriangleMesh makeCapsule( Scalar length, Scalar radius, uint nFaces ) {
    TriangleMesh result;
    result.vertices().reserve( nFaces * nFaces + nFaces + 2 );
    result.normals().reserve( nFaces * nFaces + nFaces + 2 );
    result.m_triangles.reserve( 2 * ( nFaces * nFaces + nFaces ) );

    const Scalar l = length / 2.0;

    // We create a capsule by joining a cylinder with two half spheres.

    // Part 1 : create the cylinder based on 3 circles
    // Cylinder vertices.
    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i )
    {
        const Scalar theta = i * thetaInc;
        Vector3 normal( std::cos( theta ), std::sin( theta ), 0 );

        // Create 3 circles
        Vector3 vertex = radius * normal;
        result.vertices().emplace_back( vertex[0], vertex[1], -l );
        result.vertices().emplace_back( vertex[0], vertex[1], 0.0 );
        result.vertices().emplace_back( vertex[0], vertex[1], l );

        result.normals().push_back( normal );
        result.normals().push_back( normal );
        result.normals().push_back( normal );
    }

    // Cylinder side faces
    for ( uint i = 0; i < nFaces; ++i )
    {
        uint bl = 3 * i;                      // bottom left corner of face
        uint br = 3 * ( ( i + 1 ) % nFaces ); // bottom right corner of face
        uint ml = bl + 1;                     // mid left
        uint mr = br + 1;                     // mid right
        uint tl = ml + 1;                     // top left
        uint tr = mr + 1;                     // top right

        result.m_triangles.emplace_back( bl, br, ml );
        result.m_triangles.emplace_back( br, mr, ml );

        result.m_triangles.emplace_back( ml, mr, tl );
        result.m_triangles.emplace_back( mr, tr, tl );
    }

    // Part 2 : create the bottom hemisphere.
    const Scalar phiInc = Core::Math::Pi / Scalar( nFaces );
    uint vert_count = result.vertices().size();

    // Bottom hemisphere vertices
    for ( uint j = 1; j <= nFaces / 2 - 1; ++j )
    {
        const Scalar phi = Core::Math::PiDiv2 + j * phiInc;

        for ( uint i = 0; i < nFaces; ++i )
        {
            const Scalar theta = i * thetaInc;

            const Vector3 normal( std::cos( theta ) * std::sin( phi ),
                                  std::sin( theta ) * std::sin( phi ), std::cos( phi ) );

            Vector3 vertex = radius * normal;
            vertex[2] -= l;

            result.vertices().push_back( vertex );
            result.normals().push_back( normal );
        }
    }
    // Add bottom point (south pole)
    result.vertices().emplace_back( 0, 0, -( l + radius ) );
    result.normals().emplace_back( 0, 0, -1 );

    // First ring of sphere triangles (joining with the cylinder)
    for ( uint i = 0; i < nFaces; ++i )
    {
        uint bl = 3 * i;
        uint br = 3 * ( ( i + 1 ) % nFaces );

        uint tl = vert_count + i;
        uint tr = vert_count + ( i + 1 ) % nFaces;

        result.m_triangles.emplace_back( br, bl, tl );
        result.m_triangles.emplace_back( br, tl, tr );
    }

    // Next rings of the sphere triangle
    for ( uint j = 0; j < ( nFaces / 2 ) - 2; ++j )
    {
        for ( uint i = 0; i < nFaces; ++i )
        {
            uint bl = vert_count + j * nFaces + i;
            uint br = vert_count + j * nFaces + ( i + 1 ) % nFaces;

            uint tl = vert_count + ( j + 1 ) * nFaces + i;
            uint tr = vert_count + ( j + 1 ) * nFaces + ( i + 1 ) % nFaces;

            result.m_triangles.emplace_back( br, bl, tl );
            result.m_triangles.emplace_back( br, tl, tr );
        }
    }

    // End cap triangles, joining with the pole
    for ( uint i = 0; i < nFaces; ++i )
    {
        const uint j = nFaces / 2 - 2;
        uint bl = vert_count + j * nFaces + i;
        uint br = vert_count + j * nFaces + ( i + 1 ) % nFaces;
        uint bot = result.vertices().size() - 1;
        result.m_triangles.emplace_back( br, bl, bot );
    }

    // Part 3 : create the top hemisphere
    vert_count = result.vertices().size();

    // Top hemisphere vertices
    for ( uint j = 1; j <= nFaces / 2 - 1; ++j )
    {
        const Scalar phi = Core::Math::PiDiv2 - j * phiInc;

        for ( uint i = 0; i < nFaces; ++i )
        {
            const Scalar theta = i * thetaInc;

            const Vector3 normal( std::cos( theta ) * std::sin( phi ),
                                  std::sin( theta ) * std::sin( phi ), std::cos( phi ) );

            Vector3 vertex = radius * normal;
            vertex[2] += l;

            result.vertices().push_back( vertex );
            result.normals().push_back( normal );
        }
    }

    // Add top point (north pole)
    result.vertices().emplace_back( 0, 0, ( l + radius ) );
    result.normals().emplace_back( 0, 0, 1 );

    // First ring of sphere triangles (joining with the cylinder)
    for ( uint i = 0; i < nFaces; ++i )
    {
        uint bl = 3 * i + 2;
        uint br = 3 * ( ( i + 1 ) % nFaces ) + 2;

        uint tl = vert_count + i;
        uint tr = vert_count + ( i + 1 ) % nFaces;

        result.m_triangles.emplace_back( bl, br, tl );
        result.m_triangles.emplace_back( br, tr, tl );
    }

    // Next rigns of the sphere triangles
    for ( uint j = 0; j < ( nFaces / 2 ) - 2; ++j )
    {
        for ( uint i = 0; i < nFaces; ++i )
        {
            uint bl = vert_count + j * nFaces + i;
            uint br = vert_count + j * nFaces + ( i + 1 ) % nFaces;

            uint tl = vert_count + ( j + 1 ) * nFaces + i;
            uint tr = vert_count + ( j + 1 ) * nFaces + ( i + 1 ) % nFaces;

            result.m_triangles.emplace_back( bl, br, tl );
            result.m_triangles.emplace_back( br, tr, tl );
        }
    }

    // End cap triangles, joining with the pole
    for ( uint i = 0; i < nFaces; ++i )
    {
        const uint j = nFaces / 2 - 2;
        uint bl = vert_count + j * nFaces + i;
        uint br = vert_count + j * nFaces + ( i + 1 ) % nFaces;
        uint top = result.vertices().size() - 1;
        result.m_triangles.emplace_back( bl, br, top );
    }

    // Compute normals and check results.
    checkConsistency( result );
    return result;
}

TriangleMesh makeTube( const Vector3& a, const Vector3& b, Scalar outerRadius, Scalar innerRadius,
                       uint nFaces ) {

    CORE_ASSERT( outerRadius > innerRadius, "Outer radius must be bigger than inner." );

    TriangleMesh result;
    result.vertices().reserve( 6 * nFaces );
    result.normals().reserve( 6 * nFaces );
    result.m_triangles.reserve( 12 * nFaces );

    Vector3 ab = b - a;
    Vector3 dir = ab.normalized();

    //  Create two circles normal centered on A and B and normal to ab;
    Vector3 xPlane, yPlane;
    Vector::getOrthogonalVectors( ab, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    Vector3 c = 0.5 * ( a + b );

    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i )
    {
        const Scalar theta = i * thetaInc;

        Vector3 normal = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

        result.vertices().push_back( a + outerRadius * normal );
        result.vertices().push_back( c + outerRadius * normal );
        result.vertices().push_back( b + outerRadius * normal );

        result.vertices().push_back( a + innerRadius * normal );
        result.vertices().push_back( c + innerRadius * normal );
        result.vertices().push_back( b + innerRadius * normal );

        result.normals().push_back( ( -dir + normal ).normalized() );
        result.normals().push_back( normal );
        result.normals().push_back( ( dir + normal ).normalized() );

        result.normals().push_back( ( -dir - normal ).normalized() );
        result.normals().push_back( -normal );
        result.normals().push_back( ( dir - normal ).normalized() );
    }

    for ( uint i = 0; i < nFaces; ++i )
    {
        // Outer face.
        uint obl = 6 * i;                      // bottom left corner of outer face
        uint obr = 6 * ( ( i + 1 ) % nFaces ); // bottom right corner of outer face
        uint oml = obl + 1;                    // mid left
        uint omr = obr + 1;                    // mid right
        uint otl = oml + 1;                    // top left
        uint otr = omr + 1;                    // top right

        // Inner face
        uint ibl = 6 * i + 3;                      // bottom left corner of inner face
        uint ibr = 6 * ( ( i + 1 ) % nFaces ) + 3; // bottom right corner of inner face
        uint iml = ibl + 1;                        // mid left
        uint imr = ibr + 1;                        // mid right
        uint itl = iml + 1;                        // top left
        uint itr = imr + 1;                        // top right

        // Outer face triangles, just like a regular cylinder.

        result.m_triangles.emplace_back( obl, obr, oml );
        result.m_triangles.emplace_back( obr, omr, oml );

        result.m_triangles.emplace_back( oml, omr, otl );
        result.m_triangles.emplace_back( omr, otr, otl );

        // Inner face triangles (note how order is reversed because inner face points inwards).

        result.m_triangles.emplace_back( ibr, ibl, iml );
        result.m_triangles.emplace_back( ibr, iml, imr );

        result.m_triangles.emplace_back( imr, iml, itl );
        result.m_triangles.emplace_back( imr, itl, itr );

        // Bottom face quad
        result.m_triangles.emplace_back( ibr, obr, ibl );
        result.m_triangles.emplace_back( obl, ibl, obr );

        // Top face quad
        result.m_triangles.emplace_back( otr, itr, itl );
        result.m_triangles.emplace_back( itl, otl, otr );
    }
    checkConsistency( result );
    return result;
}

TriangleMesh makeCone( const Vector3& base, const Vector3& tip, Scalar radius, uint nFaces ) {
    TriangleMesh result;
    result.vertices().reserve( 2 + nFaces );
    result.normals().reserve( 2 + nFaces );
    result.m_triangles.reserve( 2 * nFaces );

    Vector3 ab = tip - base;
    Vector3 dir = ab.normalized();

    //  Create two circles normal centered on A and B and normal to ab;
    Vector3 xPlane, yPlane;
    Vector::getOrthogonalVectors( ab, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    result.vertices().push_back( base );
    result.vertices().push_back( tip );
    result.normals().push_back( -dir );
    result.normals().push_back( dir );

    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i )
    {
        const Scalar theta = i * thetaInc;
        Vector3 normal = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

        result.vertices().push_back( base + radius * normal );
        result.normals().push_back( ( normal - dir ).normalized() );
    }

    for ( uint i = 0; i < nFaces; ++i )
    {
        uint bl = i + 2;                      // bottom left corner of face
        uint br = ( ( i + 1 ) % nFaces ) + 2; // bottom right corner of face

        result.m_triangles.emplace_back( 0, br, bl );
        result.m_triangles.emplace_back( 1, bl, br );
    }
    checkConsistency( result );
    return result;
}

TriangleMesh makePlaneGrid( const uint rows, const uint cols, const Vector2& halfExts,
                            const Transform& T ) {
    TriangleMesh grid;
    const uint R = ( rows + 1 );
    const uint C = ( cols + 1 );
    const uint v_size = C * R;
    const uint t_size = 2 * cols * rows;

    grid.vertices().resize( v_size );
    grid.normals().resize( v_size );
    grid.m_triangles.reserve( t_size );

    const Vector3 X = T.linear().col( 0 ).normalized();
    const Vector3 Y = T.linear().col( 1 ).normalized();
    const Vector3 Z = T.linear().col( 2 ).normalized();

    const Vector3 x = ( 2.0 * halfExts[0] * X ) / ( Scalar )( cols );
    const Vector3 y = ( 2.0 * halfExts[1] * Y ) / ( Scalar )( rows );
    const Vector3 o = T.translation() - ( halfExts[0] * X ) - ( halfExts[1] * Y );

    Grid<uint, 2> v( {R, C} );
    for ( uint i = 0; i < R; ++i )
    {
        for ( uint j = 0; j < C; ++j )
        {
            const uint id = ( i * C ) + j;
            v.at( {i, j} ) = id;
            grid.vertices()[id] = o + ( i * y ) + ( j * x );
            grid.normals()[id] = Z;
        }
    }

    for ( uint i = 0; i < rows; ++i )
    {
        for ( uint j = 0; j < cols; ++j )
        {
            grid.m_triangles.emplace_back( v.at( {i, j} ), v.at( {i, j + 1} ),
                                           v.at( {i + 1, j + 1} ) );
            grid.m_triangles.emplace_back( v.at( {i, j} ), v.at( {i + 1, j + 1} ),
                                           v.at( {i + 1, j} ) );
        }
    }

    return grid;
}
} // namespace MeshUtils
} // namespace Core
} // namespace Ra
