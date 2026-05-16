#include <Core/Containers/Grid.hpp>
#include <Core/Geometry/IndexedGeometry.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/StandardAttribNames.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Math.hpp> // areApproxEqual
#include <Core/Types.hpp>

#include <array>
#include <string>

namespace Ra {
namespace Core {
namespace Geometry {

QuadMesh makeXNormalQuad( const Vector2& halfExts,
                          const Utils::optional<Utils::Color>& color,
                          bool generateTexCoord ) {
    Transform T = Transform::Identity();
    T.linear().col( 0 ).swap( T.linear().col( 1 ) );
    T.linear().col( 1 ).swap( T.linear().col( 2 ) );
    return makePlaneGrid( 1, 1, halfExts, T, color, generateTexCoord );
}

QuadMesh makeYNormalQuad( const Vector2& halfExts,
                          const Utils::optional<Utils::Color>& color,
                          bool generateTexCoord ) {
    Transform T = Transform::Identity();
    T.linear().col( 1 ).swap( T.linear().col( 2 ) );
    T.linear().col( 0 ).swap( T.linear().col( 1 ) );
    return makePlaneGrid( 1, 1, halfExts, T, color, generateTexCoord );
}

QuadMesh makeZNormalQuad( const Vector2& halfExts,
                          const Utils::optional<Utils::Color>& color,
                          bool generateTexCoord ) {
    return makePlaneGrid( 1, 1, halfExts, Transform::Identity(), color, generateTexCoord );
}

TriangleMesh makeBox( const Vector3& halfExts, const Utils::optional<Utils::Color>& color ) {
    Aabb aabb( -halfExts, halfExts );
    return makeBox( aabb, color );
}

TriangleMesh makeBox( const Aabb& aabb, const Utils::optional<Utils::Color>& color ) {
    TriangleMesh result;
    result.setVertices( { aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::BottomRightFloor ),
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::TopRightFloor ),
                          aabb.corner( Aabb::BottomLeftCeil ),
                          aabb.corner( Aabb::BottomRightCeil ),
                          aabb.corner( Aabb::TopLeftCeil ),
                          aabb.corner( Aabb::TopRightCeil ) } );

    static const Scalar a = 1_ra / std::sqrt( 3_ra );

    result.setNormals( { Vector3( -a, -a, -a ),
                         Vector3( +a, -a, -a ),
                         Vector3( -a, +a, -a ),
                         Vector3( +a, +a, -a ),
                         Vector3( -a, -a, +a ),
                         Vector3( +a, -a, +a ),
                         Vector3( -a, +a, +a ),
                         Vector3( +a, +a, +a ) } );

    result.setIndices( {
        Vector3ui( 0, 2, 1 ),
        Vector3ui( 1, 2, 3 ), // Floor
        Vector3ui( 0, 1, 4 ),
        Vector3ui( 4, 1, 5 ), // Front
        Vector3ui( 3, 2, 6 ),
        Vector3ui( 3, 6, 7 ), // Back
        Vector3ui( 5, 1, 3 ),
        Vector3ui( 5, 3, 7 ), // Right
        Vector3ui( 2, 0, 4 ),
        Vector3ui( 2, 4, 6 ), // Left
        Vector3ui( 4, 5, 6 ),
        Vector3ui( 6, 5, 7 ) // Top
    } );

    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry makeBox2( const Vector3& halfExts,
                               const Utils::optional<Utils::Color>& color ) {
    return makeBox2( -halfExts,
                     Vector3 { 2_ra * halfExts.x(), 0, 0 },
                     Vector3 { 0, 2_ra * halfExts.y(), 0 },
                     Vector3 { 0, 0, 2_ra * halfExts.z() },
                     color );
}

MultiIndexedGeometry makeBox2( const Aabb& aabb, const Utils::optional<Utils::Color>& color ) {

    return makeBox2( aabb.corner( Aabb::BottomLeftFloor ),
                     aabb.corner( Aabb::BottomRightFloor ) - aabb.corner( Aabb::BottomLeftFloor ),
                     aabb.corner( Aabb::BottomLeftCeil ) - aabb.corner( Aabb::BottomLeftFloor ),
                     aabb.corner( Aabb::TopLeftFloor ) - aabb.corner( Aabb::BottomLeftFloor ),
                     color );
}

MultiIndexedGeometry makeBox2( const Vector3& corner,
                               const Vector3& x,
                               const Vector3& y,
                               const Vector3& z,
                               const Utils::optional<Utils::Color>& color ) {
    MultiIndexedGeometry result;
    result.setVertices( { corner,
                          corner + x,
                          corner + x + y,
                          corner + y,
                          corner + z,
                          corner + x + z,
                          corner + x + y + z,
                          corner + y + z } );
    const auto& v = result.vertices();

    Vector3 center = corner + .5_ra * ( x + y + z );

    result.setNormals( {
        v[0] - center,
        v[1] - center,
        v[2] - center,
        v[3] - center,
        v[4] - center,
        v[5] - center,
        v[6] - center,
        v[7] - center,
    } );

    auto layer          = std::make_unique<QuadIndexLayer>();
    layer->collection() = { Vector4ui( 0, 3, 2, 1 ),
                            Vector4ui( 4, 5, 6, 7 ),
                            Vector4ui( 0, 1, 5, 4 ),
                            Vector4ui( 0, 4, 7, 3 ),
                            Vector4ui( 1, 2, 6, 5 ),
                            Vector4ui( 2, 3, 7, 6 ) };
    if ( bool( color ) ) result.colorize( *color );

    result.addLayer( std::move( layer ) );

    return result;
}

TriangleMesh makeSharpBox( const Vector3& halfExts,
                           const Utils::optional<Utils::Color>& color,
                           bool generateTexCoord ) {
    Aabb aabb( -halfExts, halfExts );
    return makeSharpBox( aabb, color, generateTexCoord );
}

MultiIndexedGeometry makeSharpBox2( const Aabb& aabb,
                                    const Utils::optional<Utils::Color>& color,
                                    bool generateTexCoord ) {
    MultiIndexedGeometry result;
    result.setVertices( { // Floor Face
                          aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::TopRightFloor ),
                          aabb.corner( Aabb::BottomRightFloor ),

                          // Ceil Face
                          aabb.corner( Aabb::BottomLeftCeil ),
                          aabb.corner( Aabb::BottomRightCeil ),
                          aabb.corner( Aabb::TopRightCeil ),
                          aabb.corner( Aabb::TopLeftCeil ),

                          // Left Face
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::BottomLeftCeil ),
                          aabb.corner( Aabb::TopLeftCeil ),

                          // Right Face
                          aabb.corner( Aabb::BottomRightFloor ),
                          aabb.corner( Aabb::TopRightFloor ),
                          aabb.corner( Aabb::TopRightCeil ),
                          aabb.corner( Aabb::BottomRightCeil ),

                          // Bottom Face
                          aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::BottomRightFloor ),
                          aabb.corner( Aabb::BottomRightCeil ),
                          aabb.corner( Aabb::BottomLeftCeil ),

                          // Top face
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::TopLeftCeil ),
                          aabb.corner( Aabb::TopRightCeil ),
                          aabb.corner( Aabb::TopRightFloor ) } );

    result.setNormals( { // Floor face
                         Vector3( 0, 0, -1 ),
                         Vector3( 0, 0, -1 ),
                         Vector3( 0, 0, -1 ),
                         Vector3( 0, 0, -1 ),
                         // Ceil Face
                         Vector3( 0, 0, +1 ),
                         Vector3( 0, 0, +1 ),
                         Vector3( 0, 0, +1 ),
                         Vector3( 0, 0, +1 ),
                         // Left Face
                         Vector3( -1, 0, 0 ),
                         Vector3( -1, 0, 0 ),
                         Vector3( -1, 0, 0 ),
                         Vector3( -1, 0, 0 ),
                         // Right Face
                         Vector3( +1, 0, 0 ),
                         Vector3( +1, 0, 0 ),
                         Vector3( +1, 0, 0 ),
                         Vector3( +1, 0, 0 ),
                         // Bottom Face
                         Vector3( 0, -1, 0 ),
                         Vector3( 0, -1, 0 ),
                         Vector3( 0, -1, 0 ),
                         Vector3( 0, -1, 0 ),
                         // Top Face
                         Vector3( 0, +1, 0 ),
                         Vector3( 0, +1, 0 ),
                         Vector3( 0, +1, 0 ),
                         Vector3( 0, +1, 0 ) } );

    if ( generateTexCoord ) {
        Vector3Array texCoords {
            // Floor face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Ceil Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Left Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Right Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Bottom Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Top Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
        };

        result.addAttrib( "in_texcoord", std::move( texCoords ) );
    }
    {
        auto layer          = std::make_unique<QuadIndexLayer>();
        layer->collection() = {
            Vector4ui( 0, 1, 2, 3 ),     // Floor
            Vector4ui( 4, 5, 6, 7 ),     // Ceil
            Vector4ui( 8, 9, 10, 11 ),   // Left
            Vector4ui( 12, 13, 14, 15 ), // Right
            Vector4ui( 16, 17, 18, 19 ), // Bottom
            Vector4ui( 20, 21, 22, 23 )  // Top
        };

        result.addLayer( std::move( layer ) );
    }
    {
        auto layer          = std::make_unique<LineIndexLayer>();
        layer->collection() = {
            { 0, 1 },   { 1, 2 },   { 2, 3 },   { 3, 0 },   // Floor
            { 4, 5 },   { 5, 6 },   { 6, 7 },   { 7, 4 },   // Ceil
            { 8, 9 },   { 9, 10 },  { 10, 11 }, { 11, 8 },  // Left
            { 12, 13 }, { 13, 14 }, { 14, 15 }, { 15, 12 }, // Right
            { 16, 17 }, { 17, 18 }, { 18, 19 }, { 19, 16 }, // Bottom
            { 20, 21 }, { 21, 22 }, { 22, 23 }, { 23, 20 }  // Top
        };
        result.addLayer( std::move( layer ), false, "aabb" );
    }

    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();
    return result;
}

MultiIndexedGeometry makeSharpBox2( const Vector3& halfExts,
                                    const Utils::optional<Utils::Color>& color,
                                    bool generateTexCoord ) {
    Aabb aabb( -halfExts, halfExts );
    return makeSharpBox2( aabb, color, generateTexCoord );
}

TriangleMesh makeSharpBox( const Aabb& aabb,
                           const Utils::optional<Utils::Color>& color,
                           bool generateTexCoord ) {
    TriangleMesh result;
    result.setVertices( { // Floor Face
                          aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::TopRightFloor ),
                          aabb.corner( Aabb::BottomRightFloor ),

                          // Ceil Face
                          aabb.corner( Aabb::BottomLeftCeil ),
                          aabb.corner( Aabb::BottomRightCeil ),
                          aabb.corner( Aabb::TopRightCeil ),
                          aabb.corner( Aabb::TopLeftCeil ),

                          // Left Face
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::BottomLeftCeil ),
                          aabb.corner( Aabb::TopLeftCeil ),

                          // Right Face
                          aabb.corner( Aabb::BottomRightFloor ),
                          aabb.corner( Aabb::TopRightFloor ),
                          aabb.corner( Aabb::TopRightCeil ),
                          aabb.corner( Aabb::BottomRightCeil ),

                          // Bottom Face
                          aabb.corner( Aabb::BottomLeftFloor ),
                          aabb.corner( Aabb::BottomRightFloor ),
                          aabb.corner( Aabb::BottomRightCeil ),
                          aabb.corner( Aabb::BottomLeftCeil ),

                          // Top face
                          aabb.corner( Aabb::TopLeftFloor ),
                          aabb.corner( Aabb::TopLeftCeil ),
                          aabb.corner( Aabb::TopRightCeil ),
                          aabb.corner( Aabb::TopRightFloor ) } );

    result.setNormals( { // Floor face
                         Vector3( 0, 0, -1 ),
                         Vector3( 0, 0, -1 ),
                         Vector3( 0, 0, -1 ),
                         Vector3( 0, 0, -1 ),
                         // Ceil Face
                         Vector3( 0, 0, +1 ),
                         Vector3( 0, 0, +1 ),
                         Vector3( 0, 0, +1 ),
                         Vector3( 0, 0, +1 ),
                         // Left Face
                         Vector3( -1, 0, 0 ),
                         Vector3( -1, 0, 0 ),
                         Vector3( -1, 0, 0 ),
                         Vector3( -1, 0, 0 ),
                         // Right Face
                         Vector3( +1, 0, 0 ),
                         Vector3( +1, 0, 0 ),
                         Vector3( +1, 0, 0 ),
                         Vector3( +1, 0, 0 ),
                         // Bottom Face
                         Vector3( 0, -1, 0 ),
                         Vector3( 0, -1, 0 ),
                         Vector3( 0, -1, 0 ),
                         Vector3( 0, -1, 0 ),
                         // Top Face
                         Vector3( 0, +1, 0 ),
                         Vector3( 0, +1, 0 ),
                         Vector3( 0, +1, 0 ),
                         Vector3( 0, +1, 0 ) } );

    if ( generateTexCoord ) {
        Vector3Array texCoords {
            // Floor face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Ceil Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Left Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Right Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Bottom Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
            // Top Face
            Vector3( 0, 0, 0 ),
            Vector3( 1, 0, 0 ),
            Vector3( 1, 1, 0 ),
            Vector3( 0, 1, 0 ),
        };

        result.addAttrib( "in_texcoord", std::move( texCoords ) );
    }

    result.setIndices( {

        Vector3ui( 0, 1, 2 ),
        Vector3ui( 0, 2, 3 ), // Floor
        Vector3ui( 4, 5, 6 ),
        Vector3ui( 4, 6, 7 ), // Ceil
        Vector3ui( 8, 9, 10 ),
        Vector3ui( 8, 10, 11 ), // Left
        Vector3ui( 12, 13, 14 ),
        Vector3ui( 12, 14, 15 ), // Right
        Vector3ui( 16, 17, 18 ),
        Vector3ui( 16, 18, 19 ), // Bottom
        Vector3ui( 20, 21, 22 ),
        Vector3ui( 20, 22, 23 ) // Top
    } );

    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry
makeGeodesicSphere( Scalar radius, uint numSubdiv, const Utils::optional<Utils::Color>& color ) {
    MultiIndexedGeometry result;
    uint faceCount = uint( std::pow( 4, numSubdiv ) ) * 20;

    TriangleMesh::PointAttribHandle::Container vertices;
    TriangleMesh::NormalAttribHandle::Container normals;
    TriangleMesh::IndexContainerType indices;
    vertices.reserve( faceCount - 8 );
    normals.reserve( faceCount - 8 );
    indices.reserve( faceCount );

    // First, make an icosahedron.
    // Top vertex
    vertices.emplace_back( 0, 0, radius );
    normals.emplace_back( 0, 0, 1 );

    const Scalar sq5_5 = radius * std::sqrt( 5_ra ) / 5_ra;

    // Middle vertices are on pentagons inscribed on a circle of radius 2*sqrt(5)
    for ( int i = 0; i < 5; ++i ) {
        for ( int j = 0; j < 2; ++j ) {
            const Scalar theta = ( Scalar( i ) + ( j * 0.5_ra ) ) * Math::PiMul2 / 5_ra;

            const Scalar x = 2_ra * sq5_5 * std::cos( theta );
            const Scalar y = 2_ra * sq5_5 * std::sin( theta );
            const Scalar z = j == 0 ? sq5_5 : -sq5_5;
            vertices.emplace_back( x, y, z );
            normals.push_back( vertices.back().normalized() );
        }
    }

    // Bottom vertex
    vertices.emplace_back( 0, 0, -radius );
    normals.emplace_back( 0, 0, -1 );

    for ( int i = 0; i < 5; ++i ) {
        uint i1 = ( i + 1 ) % 5;
        // Top triangles
        indices.emplace_back( 0, 2 * i + 1, ( 2 * i1 + 1 ) );

        // Bottom triangles
        indices.emplace_back( 2 * i + 2, 11, ( 2 * i1 + 2 ) );
    }
    for ( uint i = 0; i < 10; ++i ) {
        uint i1 = ( i + 0 ) % 10 + 1;
        uint i2 = ( i + 1 ) % 10 + 1;
        uint i3 = ( i + 2 ) % 10 + 1;
        ( i % 2 ) ? indices.emplace_back( i3, i2, i1 ) : indices.emplace_back( i2, i3, i1 );
    }

    for ( uint n = 0; n < numSubdiv; ++n ) {
        VectorArray<Vector3ui> newTris; //= indices;
        // Now subdivide every face into 4 triangles.
        for ( uint i = 0; i < indices.size(); ++i ) {
            const Vector3ui& tri               = indices[i];
            std::array<Vector3, 3> triVertices = {
                { vertices[tri[0]], vertices[tri[1]], vertices[tri[2]] } };
            std::array<uint, 3> middles;

            for ( uint v = 0; v < 3; ++v ) {
                middles[v] = uint( vertices.size() );

                Vector3 vertex = 0.5_ra * ( triVertices[v] + triVertices[( v + 1 ) % 3] );
                vertex.normalize();

                vertices.push_back( radius * vertex );
                normals.push_back( vertex );
            }

            newTris.emplace_back( tri[0], middles[0], middles[2] );
            newTris.emplace_back( middles[0], tri[1], middles[1] );
            newTris.emplace_back( middles[2], middles[1], tri[2] );
            newTris.emplace_back( middles[0], middles[1], middles[2] );
        }
        indices = newTris;
    }

    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.set_indices<TriangleIndexLayer>( std::move( indices ) );
    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry makeCylinder( const Vector3& a,
                                   const Vector3& b,
                                   Scalar radius,
                                   uint sideSegments,
                                   uint fillSegments,
                                   const Utils::optional<Utils::Color>& color ) {
    MultiIndexedGeometry result;

    MultiIndexedGeometry::PointAttribHandle::Container vertices;
    MultiIndexedGeometry::NormalAttribHandle::Container normals;
    TriangleIndexLayer::IndexContainerType indices;

    const Vector3 ab  = b - a;
    const Vector3 dir = ab.normalized();

    //  Create two circles normal centered on A and B and normal to ab (use dir, since first vector
    //  must be normalized)
    Vector3 xPlane, yPlane;
    Math::getOrthogonalVectors( dir, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    vertices.push_back( a );
    normals.push_back( -dir );
    vertices.push_back( b );
    normals.push_back( dir );

    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( sideSegments ) );
    for ( uint i = 0; i < sideSegments; ++i ) {
        const Scalar theta   = i * thetaInc;
        const Vector3 normal = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

        // Even indices are A circle and odd indices are B circle.
        vertices.push_back( a + radius * normal );
        normals.push_back( -dir );

        vertices.push_back( b + radius * normal );
        normals.push_back( dir );
    }

    for ( uint i = 0; i < sideSegments; ++i ) {
        uint bl = 2 * i + 2;
        uint br = 2 + ( 2 * ( ( i + 1 ) % sideSegments ) );
        uint tl = 2 * i + 3;
        uint tr = 3 + ( 2 * ( ( i + 1 ) % sideSegments ) );
        // order consistency (ccw face) here is important, e.g. when creating topomesh
        indices.emplace_back( 0, br, bl );
        indices.emplace_back( 1, tl, tr );
    }

    // sew tube between circles.
    const uint offset = vertices.size();
    Vector3 c         = a;
    const Vector3 dh  = ab / Scalar( fillSegments );
    for ( uint j = 0; j <= fillSegments; ++j ) {
        for ( uint i = 0; i < sideSegments; ++i ) {
            const Scalar theta = i * thetaInc;
            Vector3 normal     = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

            vertices.push_back( c + radius * normal );
            normals.push_back( normal );
        }
        c += dh;
    }

    for ( uint j = 0; j < fillSegments; ++j ) {
        for ( uint i = 0; i < sideSegments; ++i ) {
            uint i0 = offset + i + j * sideSegments;
            uint i1 = offset + ( i + 1 ) % sideSegments + j * sideSegments;
            uint i2 = i0 + sideSegments;
            uint i3 = i1 + sideSegments;

            indices.emplace_back( i0, i1, i2 );
            indices.emplace_back( i2, i1, i3 );
        }
    }

    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.set_indices<TriangleIndexLayer>( std::move( indices ) );
    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry makeCapsule( Scalar length,
                                  Scalar radius,
                                  uint nFaces,
                                  const Utils::optional<Utils::Color>& color ) {
    MultiIndexedGeometry result;

    TriangleMesh::PointAttribHandle::Container vertices;
    TriangleMesh::NormalAttribHandle::Container normals;
    TriangleMesh::IndexContainerType indices;

    vertices.reserve( nFaces * nFaces + nFaces + 2 );
    normals.reserve( nFaces * nFaces + nFaces + 2 );
    indices.reserve( 2 * ( nFaces * nFaces + nFaces ) );

    const Scalar l = length / 2_ra;

    // We create a capsule by joining a cylinder with two half spheres.

    // Part 1 : create the cylinder based on 3 circles
    // Cylinder vertices.
    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i ) {
        const Scalar theta = i * thetaInc;
        Vector3 normal( std::cos( theta ), std::sin( theta ), 0 );

        // Create 3 circles
        Vector3 vertex = radius * normal;
        vertices.emplace_back( vertex[0], vertex[1], -l );
        vertices.emplace_back( vertex[0], vertex[1], 0.0 );
        vertices.emplace_back( vertex[0], vertex[1], l );

        normals.push_back( normal );
        normals.push_back( normal );
        normals.push_back( normal );
    }

    // Cylinder side faces
    for ( uint i = 0; i < nFaces; ++i ) {
        uint bl = 3 * i;                      // bottom left corner of face
        uint br = 3 * ( ( i + 1 ) % nFaces ); // bottom right corner of face
        uint ml = bl + 1;                     // mid left
        uint mr = br + 1;                     // mid right
        uint tl = ml + 1;                     // top left
        uint tr = mr + 1;                     // top right

        indices.emplace_back( bl, br, ml );
        indices.emplace_back( br, mr, ml );

        indices.emplace_back( ml, mr, tl );
        indices.emplace_back( mr, tr, tl );
    }

    // Part 2 : create the bottom hemisphere.
    const Scalar phiInc = Core::Math::Pi / Scalar( nFaces );
    uint vert_count     = uint( vertices.size() );

    // Bottom hemisphere vertices
    for ( uint j = 1; j <= nFaces / 2 - 1; ++j ) {
        const Scalar phi = Core::Math::PiDiv2 + j * phiInc;

        for ( uint i = 0; i < nFaces; ++i ) {
            const Scalar theta = i * thetaInc;

            const Vector3 normal( std::cos( theta ) * std::sin( phi ),
                                  std::sin( theta ) * std::sin( phi ),
                                  std::cos( phi ) );

            Vector3 vertex = radius * normal;
            vertex[2] -= l;

            vertices.push_back( vertex );
            normals.push_back( normal );
        }
    }
    // Add bottom point (south pole)
    vertices.emplace_back( 0, 0, -( l + radius ) );
    normals.emplace_back( 0, 0, -1 );

    // First ring of sphere triangles (joining with the cylinder)
    for ( uint i = 0; i < nFaces; ++i ) {
        uint bl = 3 * i;
        uint br = 3 * ( ( i + 1 ) % nFaces );

        uint tl = vert_count + i;
        uint tr = vert_count + ( i + 1 ) % nFaces;

        indices.emplace_back( br, bl, tl );
        indices.emplace_back( br, tl, tr );
    }

    // Next rings of the sphere triangles
    for ( uint j = 0; j < ( nFaces / 2 ) - 2; ++j ) {
        for ( uint i = 0; i < nFaces; ++i ) {
            uint bl = vert_count + j * nFaces + i;
            uint br = vert_count + j * nFaces + ( i + 1 ) % nFaces;

            uint tl = vert_count + ( j + 1 ) * nFaces + i;
            uint tr = vert_count + ( j + 1 ) * nFaces + ( i + 1 ) % nFaces;

            indices.emplace_back( br, bl, tl );
            indices.emplace_back( br, tl, tr );
        }
    }

    // End cap triangles, joining with the pole
    for ( uint i = 0; i < nFaces; ++i ) {
        const uint j = nFaces / 2 - 2;
        uint bl      = vert_count + j * nFaces + i;
        uint br      = vert_count + j * nFaces + ( i + 1 ) % nFaces;
        uint bot     = uint( vertices.size() - 1 );
        indices.emplace_back( br, bl, bot );
    }

    // Part 3 : create the top hemisphere
    vert_count = uint( vertices.size() );

    // Top hemisphere vertices
    for ( uint j = 1; j <= nFaces / 2 - 1; ++j ) {
        const Scalar phi = Core::Math::PiDiv2 - j * phiInc;

        for ( uint i = 0; i < nFaces; ++i ) {
            const Scalar theta = i * thetaInc;

            const Vector3 normal( std::cos( theta ) * std::sin( phi ),
                                  std::sin( theta ) * std::sin( phi ),
                                  std::cos( phi ) );

            Vector3 vertex = radius * normal;
            vertex[2] += l;

            vertices.push_back( vertex );
            normals.push_back( normal );
        }
    }

    // Add top point (north pole)
    vertices.emplace_back( 0, 0, ( l + radius ) );
    normals.emplace_back( 0, 0, 1 );

    // First ring of sphere triangles (joining with the cylinder)
    for ( uint i = 0; i < nFaces; ++i ) {
        uint bl = 3 * i + 2;
        uint br = 3 * ( ( i + 1 ) % nFaces ) + 2;

        uint tl = vert_count + i;
        uint tr = vert_count + ( i + 1 ) % nFaces;

        indices.emplace_back( bl, br, tl );
        indices.emplace_back( br, tr, tl );
    }

    // Next rigns of the sphere triangles
    for ( uint j = 0; j < ( nFaces / 2 ) - 2; ++j ) {
        for ( uint i = 0; i < nFaces; ++i ) {
            uint bl = vert_count + j * nFaces + i;
            uint br = vert_count + j * nFaces + ( i + 1 ) % nFaces;

            uint tl = vert_count + ( j + 1 ) * nFaces + i;
            uint tr = vert_count + ( j + 1 ) * nFaces + ( i + 1 ) % nFaces;

            indices.emplace_back( bl, br, tl );
            indices.emplace_back( br, tr, tl );
        }
    }

    // End cap triangles, joining with the pole
    for ( uint i = 0; i < nFaces; ++i ) {
        const uint j = nFaces / 2 - 2;
        uint bl      = vert_count + j * nFaces + i;
        uint br      = vert_count + j * nFaces + ( i + 1 ) % nFaces;
        uint top     = uint( vertices.size() ) - 1;
        indices.emplace_back( bl, br, top );
    }
    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.set_indices<TriangleIndexLayer>( std::move( indices ) );
    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry makeTube( const Vector3& a,
                               const Vector3& b,
                               Scalar outerRadius,
                               Scalar innerRadius,
                               uint nFaces,
                               const Utils::optional<Utils::Color>& color ) {

    CORE_ASSERT( outerRadius > innerRadius, "Outer radius must be bigger than inner." );

    MultiIndexedGeometry result;
    MultiIndexedGeometry::PointAttribHandle::Container vertices;
    MultiIndexedGeometry::NormalAttribHandle::Container normals;
    TriangleIndexLayer::IndexContainerType indices;
    vertices.reserve( 6 * nFaces );
    normals.reserve( 6 * nFaces );
    indices.reserve( 12 * nFaces );

    Vector3 ab  = b - a;
    Vector3 dir = ab.normalized();

    //  Create two circles normal centered on A and B and normal to ab;
    Vector3 xPlane, yPlane;
    Math::getOrthogonalVectors( dir, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    Vector3 c = 0.5 * ( a + b );

    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i ) {
        const Scalar theta = i * thetaInc;

        Vector3 normal = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

        vertices.push_back( a + outerRadius * normal );
        vertices.push_back( c + outerRadius * normal );
        vertices.push_back( b + outerRadius * normal );

        vertices.push_back( a + innerRadius * normal );
        vertices.push_back( c + innerRadius * normal );
        vertices.push_back( b + innerRadius * normal );

        normals.push_back( ( -dir + normal ).normalized() );
        normals.push_back( normal );
        normals.push_back( ( dir + normal ).normalized() );

        normals.push_back( ( -dir - normal ).normalized() );
        normals.push_back( -normal );
        normals.push_back( ( dir - normal ).normalized() );
    }

    for ( uint i = 0; i < nFaces; ++i ) {
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

        indices.emplace_back( obl, obr, oml );
        indices.emplace_back( obr, omr, oml );

        indices.emplace_back( oml, omr, otl );
        indices.emplace_back( omr, otr, otl );

        // Inner face triangles (note how order is reversed because inner face points inwards).

        indices.emplace_back( ibr, ibl, iml );
        indices.emplace_back( ibr, iml, imr );

        indices.emplace_back( imr, iml, itl );
        indices.emplace_back( imr, itl, itr );

        // Bottom face quad
        indices.emplace_back( ibr, obr, ibl );
        indices.emplace_back( obl, ibl, obr );

        // Top face quad
        indices.emplace_back( otr, itr, itl );
        indices.emplace_back( itl, otl, otr );
    }
    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.set_indices<TriangleIndexLayer>( std::move( indices ) );
    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry makeCone( const Vector3& base,
                               const Vector3& tip,
                               Scalar radius,
                               uint nFaces,
                               const Utils::optional<Utils::Color>& color ) {
    MultiIndexedGeometry result;
    MultiIndexedGeometry::PointAttribHandle::Container vertices;
    MultiIndexedGeometry::NormalAttribHandle::Container normals;
    TriangleIndexLayer::IndexContainerType indices;
    vertices.reserve( 2 + nFaces );
    normals.reserve( 2 + nFaces );
    indices.reserve( 2 * nFaces );

    Vector3 ab  = tip - base;
    Vector3 dir = ab.normalized();

    //  Create two circles normal centered on A and B and normal to ab;
    Vector3 xPlane, yPlane;
    Math::getOrthogonalVectors( dir, xPlane, yPlane );
    xPlane.normalize();
    yPlane.normalize();

    vertices.push_back( base );
    vertices.push_back( tip );
    normals.push_back( -dir );
    normals.push_back( dir );

    const Scalar thetaInc( Core::Math::PiMul2 / Scalar( nFaces ) );
    for ( uint i = 0; i < nFaces; ++i ) {
        const Scalar theta = i * thetaInc;
        Vector3 normal     = std::cos( theta ) * xPlane + std::sin( theta ) * yPlane;

        vertices.push_back( base + radius * normal );
        normals.push_back( ( normal - dir ).normalized() );
    }

    for ( uint i = 0; i < nFaces; ++i ) {
        uint bl = i + 2;                      // bottom left corner of face
        uint br = ( ( i + 1 ) % nFaces ) + 2; // bottom right corner of face

        indices.emplace_back( 0, br, bl );
        indices.emplace_back( 1, bl, br );
    }
    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.set_indices<TriangleIndexLayer>( std::move( indices ) );
    if ( bool( color ) ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

LineMesh makeGrid( const Core::Vector3& center,
                   const Core::Vector3& x,
                   const Core::Vector3& y,
                   const Core::Utils::Color& color,
                   Scalar cell_size,
                   uint res ) {

    CORE_ASSERT( res > 1, "Grid has to be at least a 2x2 grid." );

    LineMesh result;
    TriangleMesh::PointAttribHandle::Container vertices;
    auto indices = LineMesh::IndexContainerType {};

    vertices.reserve( 4 * ( res + 1 ) );
    indices.reserve( 2 * ( res + 1 ) );

    const Scalar halfWidth { ( cell_size * res ) / 2.f };
    const Core::Vector3 deltaPosX { cell_size * x };
    const Core::Vector3 startPosX { center - halfWidth * x };
    const Core::Vector3 endPosX { center + halfWidth * x };
    const Core::Vector3 deltaPosY { cell_size * y };
    const Core::Vector3 startPosY { center - halfWidth * y };
    const Core::Vector3 endPosY { center + halfWidth * y };
    Core::Vector3 currentPosX { startPosX };
    for ( uint i = 0; i < res + 1; ++i ) {
        vertices.emplace_back( startPosY + currentPosX );
        vertices.emplace_back( endPosY + currentPosX );
        indices.emplace_back( uint( vertices.size() ) - 2, uint( vertices.size() ) - 1 );
        currentPosX += deltaPosX;
    }

    Core::Vector3 currentPosY = startPosY;
    for ( uint i = 0; i < res + 1; ++i ) {
        vertices.emplace_back( startPosX + currentPosY );
        vertices.emplace_back( endPosX + currentPosY );
        indices.emplace_back( uint( vertices.size() ) - 2, uint( vertices.size() ) - 1 );
        currentPosY += deltaPosY;
    }

    result.setVertices( std::move( vertices ) );
    result.setIndices( std::move( indices ) );

    result.colorize( color );
    return result;
}

QuadMesh makePlaneGrid( const uint rows,
                        const uint cols,
                        const Vector2& halfExts,
                        const Transform& transform,
                        const Utils::optional<Utils::Color>& color,
                        bool generateTexCoord ) {
    QuadMesh result;

    QuadMesh::PointAttribHandle::Container vertices;
    QuadMesh::NormalAttribHandle::Container normals;
    Ra::Core::Vector3Array texCoords;

    const uint R      = ( rows + 1 );
    const uint C      = ( cols + 1 );
    const uint v_size = C * R;
    const uint f_size = cols * rows;

    vertices.resize( v_size );
    normals.resize( v_size );
    texCoords.resize( v_size );

    const Vector3 X = transform.linear().col( 0 ).normalized();
    const Vector3 Y = transform.linear().col( 1 ).normalized();
    const Vector3 Z = transform.linear().col( 2 ).normalized();

    const Vector3 x      = ( 2_ra * halfExts[0] * X ) / Scalar( cols );
    const Vector3 y      = ( 2_ra * halfExts[1] * Y ) / Scalar( rows );
    const Vector3 origin = transform.translation() - ( halfExts[0] * X ) - ( halfExts[1] * Y );

    const Scalar du = 1_ra / cols;
    const Scalar dv = 1_ra / rows;

    Grid<uint, 2> v( { R, C } );
    Scalar cv           = 0;
    Vector3 current_row = origin;
    uint id             = 0;
    for ( uint i = 0; i < R; ++i ) {
        Scalar cu             = 0;
        Vector3 current_point = current_row;
        for ( uint j = 0; j < C; ++j ) {
            v.at( { i, j } ) = id;
            vertices[id]     = current_point;
            normals[id]      = Z;
            texCoords[id]    = Vector3( cu, cv, 0_ra );
            current_point += x;
            cu += du;
            ++id;
        }
        cv += dv;
        current_row += y;
    }

    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    if ( generateTexCoord )
        result.addAttrib( getAttribName( MeshAttrib::VERTEX_TEXCOORD ), std::move( texCoords ) );
    if ( bool( color ) ) result.colorize( *color );

    auto& face_layer = result.getIndicesWithLock();
    face_layer.reserve( f_size );

    for ( uint i = 0; i < rows; ++i ) {
        for ( uint j = 0; j < cols; ++j ) {
            face_layer.emplace_back( v.at( { i, j } ),
                                     v.at( { i, j + 1 } ),
                                     v.at( { i + 1, j + 1 } ),
                                     v.at( { i + 1, j } ) );
        }
    }

    result.indicesUnlock();
    result.checkConsistency();

    return result;
}

MultiIndexedGeometry makeParametricSphere( Scalar radius,
                                           const Utils::optional<Utils::Color>& color,
                                           bool gtc,
                                           const uint SLICES,
                                           const uint STACKS ) {

    const Scalar du = 1_ra / SLICES;
    const Scalar dv = 1_ra / STACKS;
    using WAI       = TopologicalMesh::WedgeAttribIndex;
    TopologicalMesh topoMesh;
    auto whNormal = topoMesh.addWedgeAttrib<Vector3>( getAttribName( MeshAttrib::VERTEX_NORMAL ) );

    WAI whColor =
        color ? topoMesh.addWedgeAttrib<Vector4>( getAttribName( MeshAttrib::VERTEX_COLOR ) )
              : WAI::Invalid();
    WAI whTexCoord =
        gtc ? topoMesh.addWedgeAttrib<Vector3>( getAttribName( MeshAttrib::VERTEX_TEXCOORD ) )
            : WAI::Invalid();

    std::vector<TopologicalMesh::VertexHandle> vhandles;
    vhandles.resize( ( STACKS - 1 ) * SLICES + 2 );
    Vector3Array topoTexCoords;
    topoTexCoords.reserve( ( STACKS - 1 ) * SLICES + 2 + 2 * SLICES );
    Vector3Array topoNormals;
    topoNormals.reserve( ( STACKS - 1 ) * SLICES + 2 );

    uint index = 0;
    // check https://en.wikipedia.org/wiki/Spherical_coordinate_system
    // theta \in [0, pi]
    // phi \in [0, 2pi]
    const Scalar uFactor = 2_ra / static_cast<Scalar>( SLICES ) * Core::Math::Pi;
    const Scalar vFactor = Core::Math::Pi / STACKS;
    for ( uint u = 0; u < SLICES; ++u ) {
        const Scalar phi = u * uFactor;
        for ( uint v = 1; v < STACKS; ++v ) {
            // Regular vertices on the sphere.
            const Scalar theta = v * vFactor;
            Vector3 p          = Vector3( radius * std::cos( phi ) * std::sin( theta ),
                                          radius * std::sin( phi ) * std::sin( theta ),
                                          radius * std::cos( theta ) );
            vhandles[index++]  = topoMesh.add_vertex( p );
            topoNormals.push_back( p.normalized() );
            topoTexCoords.emplace_back( 1_ra - u * du, v * dv, 0_ra );
        }
    }

    // Add the pole vertices.
    auto topoNorthPoleIdx      = ( STACKS - 1 ) * SLICES;
    auto topoSouthPoleIdx      = topoNorthPoleIdx + 1;
    vhandles[topoNorthPoleIdx] = topoMesh.add_vertex( Vector3( 0, 0, radius ) );
    vhandles[topoSouthPoleIdx] = topoMesh.add_vertex( Vector3( 0, 0, -radius ) );

    // Set wedge for fh, vhIndex to n and t.
    auto wedgeSetter = [&topoMesh, &vhandles, color, whNormal, whTexCoord, whColor, gtc](
                           int vhIndex, TopologicalMesh::FaceHandle fh, Vector3 n, Vector3 t ) {
        auto heh                     = topoMesh.halfedge_handle( vhandles[vhIndex], fh );
        auto wd                      = topoMesh.newWedgeData( heh );
        wd.m_vector3Attrib[whNormal] = n;
        if ( gtc ) wd.m_vector3Attrib[whTexCoord] = t;
        if ( color ) wd.m_vector4Attrib[whColor] = *color;
        topoMesh.replaceWedge( heh, wd );
    };

    // For general vertices retrieve normals and texCoords from vhIndex
    auto wedgeSetterGeneric = [wedgeSetter, &topoNormals, &topoTexCoords, whNormal, whTexCoord](
                                  int vhIndex, TopologicalMesh::FaceHandle fh ) {
        wedgeSetter( vhIndex, fh, topoNormals[vhIndex], topoTexCoords[vhIndex] );
    };

    // take seams into account when u =1
    auto wedgeSetterSeam =
        [wedgeSetter, &topoTexCoords, &topoNormals, whNormal, whTexCoord, SLICES](
            uint u, int vhIndex, TopologicalMesh::FaceHandle fh ) {
            Vector3 t = topoTexCoords[vhIndex];
            if ( u == SLICES - 1 ) t[0] = 0_ra;
            wedgeSetter( vhIndex, fh, topoNormals[vhIndex], t );
        };

    // special for poles
    auto wedgeSetterPole = [wedgeSetter, &topoTexCoords, whNormal, whTexCoord, SLICES](
                               bool north,
                               int id,
                               int baseSlice,
                               int nextSlice,
                               uint u,
                               TopologicalMesh::FaceHandle fh ) {
        // pole vertex use "midpoint" texCoord
        Scalar bu = topoTexCoords[baseSlice][0];
        Scalar nu = ( u == SLICES - 1 ) ? 0_ra : topoTexCoords[nextSlice][0];
        Scalar tu = ( bu + nu ) * .5_ra;
        wedgeSetter( id, fh, Vector3( 0, 0, north ? 1 : -1 ), Vector3( tu, north ? 0 : 1, 0 ) );
    };

    for ( uint u = 0; u < SLICES; ++u ) {
        for ( uint v = 2; v < STACKS; ++v ) {
            const uint nextSlice = ( ( u + 1 ) % SLICES ) * ( STACKS - 1 );
            const uint baseSlice = u * ( STACKS - 1 );
            std::vector vindices = { baseSlice + v - 2,
                                     baseSlice + v - 1,
                                     nextSlice + v - 1,
                                     baseSlice + v - 2,
                                     nextSlice + v - 1,
                                     nextSlice + v - 2 };

            auto fh1 = topoMesh.add_face(
                { vhandles[vindices[0]], vhandles[vindices[1]], vhandles[vindices[2]] } );
            auto fh2 = topoMesh.add_face(
                { vhandles[vindices[3]], vhandles[vindices[4]], vhandles[vindices[5]] } );

            wedgeSetterGeneric( baseSlice + v - 2, fh1 );
            wedgeSetterGeneric( baseSlice + v - 1, fh1 );
            wedgeSetterGeneric( baseSlice + v - 2, fh2 );

            wedgeSetterSeam( u, nextSlice + v - 1, fh1 );
            wedgeSetterSeam( u, nextSlice + v - 1, fh2 );
            wedgeSetterSeam( u, nextSlice + v - 2, fh2 );
        }
    }
    // caps faces
    for ( uint u = 0; u < SLICES; ++u ) {
        const uint nextSlice = ( ( u + 1 ) % SLICES ) * ( STACKS - 1 );
        const uint baseSlice = u * ( STACKS - 1 );

        auto fh1 = topoMesh.add_face(
            { vhandles[topoNorthPoleIdx], vhandles[baseSlice], vhandles[nextSlice] } );
        auto fh2 = topoMesh.add_face( { vhandles[topoSouthPoleIdx],
                                        vhandles[nextSlice + STACKS - 2],
                                        vhandles[baseSlice + STACKS - 2] } );

        wedgeSetterGeneric( baseSlice, fh1 );
        wedgeSetterGeneric( baseSlice + STACKS - 2, fh2 );
        wedgeSetterSeam( u, nextSlice, fh1 );
        wedgeSetterSeam( u, nextSlice + STACKS - 2, fh2 );

        // pole vertex use "midpoint" texCoord
        wedgeSetterPole( true, topoNorthPoleIdx, baseSlice, nextSlice, u, fh1 );
        wedgeSetterPole( false, topoSouthPoleIdx, baseSlice, nextSlice, u, fh2 );
    }

    topoMesh.mergeEqualWedges();
    topoMesh.garbage_collection();
    auto result = topoMesh.toTriangleMesh();
    result.checkConsistency();
    return result;
}

MultiIndexedGeometry makeParametricTorus( Scalar majorRadius,
                                          Scalar minorRadius,
                                          const Utils::optional<Utils::Color>& color,
                                          bool generateTexCoord,
                                          const uint U,
                                          const uint V ) {
    MultiIndexedGeometry result;
    MultiIndexedGeometry::PointAttribHandle::Container vertices;
    MultiIndexedGeometry::NormalAttribHandle::Container normals;
    QuadIndexLayer::IndexContainerType indices;
    Ra::Core::Vector3Array texCoords;

    vertices.reserve( ( U + 1 ) * ( V + 1 ) );
    normals.reserve( ( U + 1 ) * ( V + 1 ) );
    indices.reserve( 2 * U * V );
    texCoords.reserve( ( U + 1 ) * ( V + 1 ) );

    const Scalar du = 1_ra / U;
    const Scalar dv = 1_ra / V;

    for ( uint iu = 0; iu <= U; ++iu ) {
        Scalar u = Scalar( iu ) * Core::Math::PiMul2 / Scalar( U );
        Core::Vector3 circleCenter( majorRadius * std::cos( u ), majorRadius * std::sin( u ), 0.f );

        for ( uint iv = 0; iv <= V; ++iv ) {
            Scalar v = Scalar( iv ) * Core::Math::PiMul2 / Scalar( V );

            Core::Vector3 vertex( ( majorRadius + minorRadius * std::cos( v ) ) * std::cos( u ),
                                  ( majorRadius + minorRadius * std::cos( v ) ) * std::sin( u ),
                                  minorRadius * std::sin( v ) );

            vertices.push_back( vertex );
            normals.push_back( ( vertex - circleCenter ).normalized() );
            texCoords.emplace_back( iu * du, iv * dv, 0_ra );

            if ( iu != U && iv != V ) {
                indices.emplace_back( iu * ( V + 1 ) + iv,
                                      ( iu + 1 ) * ( V + 1 ) + iv,
                                      ( iu + 1 ) * ( V + 1 ) + ( iv + 1 ),
                                      iu * ( V + 1 ) + ( iv + 1 ) );
            }
        }
    }

    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.set_indices<QuadIndexLayer>( std::move( indices ) );
    if ( generateTexCoord )
        result.addAttrib( getAttribName( MeshAttrib::VERTEX_TEXCOORD ), std::move( texCoords ) );
    if ( color ) result.colorize( *color );
    result.checkConsistency();

    return result;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
