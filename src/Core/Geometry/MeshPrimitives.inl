#pragma once
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Math/LinearAlgebra.hpp> // Math::pi

#include <random>

namespace Ra {
namespace Core {
namespace Geometry {

template <uint SLICES, uint STACKS>
TriangleMesh
makeParametricSphere( Scalar radius, const Utils::optional<Utils::Color>& color, bool gtc ) {

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

    TopologicalMesh::VertexHandle vhandles[( STACKS - 1 ) * SLICES + 2];
    Vector3Array topoTexCoords;
    topoTexCoords.reserve( ( STACKS - 1 ) * SLICES + 2 + 2 * SLICES );
    Vector3Array topoNormals;
    topoNormals.reserve( ( STACKS - 1 ) * SLICES + 2 );

    uint index = 0;
    // check https://en.wikipedia.org/wiki/Spherical_coordinate_system
    // theta \in [0, pi]
    // phi \in [0, 2pi]
    const Scalar uFactor = 2_ra / Scalar { SLICES } * Core::Math::Pi;
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
    auto wedgeSetterSeam = [wedgeSetter, &topoTexCoords, &topoNormals, whNormal, whTexCoord](
                               int u, int vhIndex, TopologicalMesh::FaceHandle fh ) {
        Vector3 t = topoTexCoords[vhIndex];
        if ( u == SLICES - 1 ) t[0] = 0_ra;
        wedgeSetter( vhIndex, fh, topoNormals[vhIndex], t );
    };

    // special for poles
    auto wedgeSetterPole =
        [wedgeSetter, &topoTexCoords, whNormal, whTexCoord]( bool north,
                                                             int id,
                                                             int baseSlice,
                                                             int nextSlice,
                                                             int u,
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
    TriangleMesh result = topoMesh.toTriangleMesh();
    result.checkConsistency();
    return result;
}

template <uint U, uint V>
TriangleMesh makeParametricTorus( Scalar majorRadius,
                                  Scalar minorRadius,
                                  const Utils::optional<Utils::Color>& color,
                                  bool generateTexCoord ) {
    TriangleMesh result;
    TriangleMesh::PointAttribHandle::Container vertices;
    TriangleMesh::NormalAttribHandle::Container normals;
    TriangleMesh::IndexContainerType indices;
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
                indices.push_back( Vector3ui( iu * ( V + 1 ) + iv,
                                              ( iu + 1 ) * ( V + 1 ) + iv,
                                              iu * ( V + 1 ) + ( iv + 1 ) ) );
                indices.push_back( Vector3ui( ( iu + 1 ) * ( V + 1 ) + iv,
                                              ( iu + 1 ) * ( V + 1 ) + ( iv + 1 ),
                                              iu * ( V + 1 ) + ( iv + 1 ) ) );
            }
        }
    }

    result.setVertices( std::move( vertices ) );
    result.setNormals( std::move( normals ) );
    result.setIndices( std::move( indices ) );
    if ( generateTexCoord )
        result.addAttrib( getAttribName( MeshAttrib::VERTEX_TEXCOORD ), std::move( texCoords ) );
    if ( color ) result.colorize( *color );
    result.checkConsistency();

    return result;
}
} // namespace Geometry
} // namespace Core
} // namespace Ra
