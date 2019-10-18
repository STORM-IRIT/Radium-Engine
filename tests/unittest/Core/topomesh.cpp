#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <catch2/catch.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>

bool isSameMesh( Ra::Core::Geometry::TriangleMesh& meshOne,
                 Ra::Core::Geometry::TriangleMesh& meshTwo ) {

    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TopologicalMesh;
    using Ra::Core::Geometry::TriangleMesh;

    bool result = true;
    int i       = 0;
    // Check length
    if ( meshOne.vertices().size() != meshTwo.vertices().size() ) return false;
    if ( meshOne.normals().size() != meshTwo.normals().size() ) return false;
    if ( meshOne.m_indices.size() != meshTwo.m_indices.size() ) return false;

    // Check triangles
    std::vector<Ra::Core::Vector3> stackVertices;
    std::vector<Ra::Core::Vector3> stackNormals;

    i = 0;
    while ( result && i < int( meshOne.m_indices.size() ) )
    {
        std::vector<Ra::Core::Vector3>::iterator it;
        stackVertices.clear();
        stackVertices.push_back( meshOne.vertices()[meshOne.m_indices[i][0]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.m_indices[i][1]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.m_indices[i][2]] );

        stackNormals.clear();
        stackNormals.push_back( meshOne.normals()[meshOne.m_indices[i][0]] );
        stackNormals.push_back( meshOne.normals()[meshOne.m_indices[i][1]] );
        stackNormals.push_back( meshOne.normals()[meshOne.m_indices[i][2]] );

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackVertices.begin(),
                       stackVertices.end(),
                       meshTwo.vertices()[meshTwo.m_indices[i][j]] );
            if ( it != stackVertices.end() ) { stackVertices.erase( it ); }
            else
            { result = false; }
        }

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackNormals.begin(),
                       stackNormals.end(),
                       meshTwo.normals()[meshTwo.m_indices[i][j]] );
            if ( it != stackNormals.end() ) { stackNormals.erase( it ); }
            else
            { result = false; }
        }

        ++i;
    }
    return result;
}

TEST_CASE( "Core/Geometry/TopologicalMesh", "[Core][Core/Geometry][TopologicalMesh]" ) {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TopologicalMesh;
    using Ra::Core::Geometry::TriangleMesh;

    using Catmull =
        OpenMesh::Subdivider::Uniform::CatmullClarkT<Ra::Core::Geometry::TopologicalMesh>;
    using Loop = OpenMesh::Subdivider::Uniform::LoopT<Ra::Core::Geometry::TopologicalMesh>;

    using Decimater = OpenMesh::Decimater::DecimaterT<Ra::Core::Geometry::TopologicalMesh>;
    using HModQuadric =
        OpenMesh::Decimater::ModQuadricT<Ra::Core::Geometry::TopologicalMesh>::Handle;

    TriangleMesh newMesh;
    TriangleMesh mesh;
    TopologicalMesh topologicalMesh;

    // Test for close mesh
    mesh            = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( isSameMesh( mesh, newMesh ) );

    mesh            = Ra::Core::Geometry::makeSharpBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( isSameMesh( mesh, newMesh ) );

    // Test for mesh with boundaries
    mesh            = Ra::Core::Geometry::makePlaneGrid( 2, 2 );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( isSameMesh( mesh, newMesh ) );

    mesh            = Ra::Core::Geometry::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( isSameMesh( mesh, newMesh ) );

    // Test skip empty attributes
    mesh.addAttrib<float>( "empty" );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh         = topologicalMesh.toTriangleMesh();
    REQUIRE( !newMesh.hasAttrib( "empty" ) );

    // Test normals
    mesh            = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );

    for ( TopologicalMesh::ConstVertexIter v_it = topologicalMesh.vertices_begin();
          v_it != topologicalMesh.vertices_end();
          ++v_it )
    {
        topologicalMesh.set_normal(
            *v_it, TopologicalMesh::Normal( Scalar( 1. ), Scalar( 0. ), Scalar( 0. ) ) );
    }

    for ( TopologicalMesh::ConstVertexIter v_it = topologicalMesh.vertices_begin();
          v_it != topologicalMesh.vertices_end();
          ++v_it )
    {
        topologicalMesh.propagate_normal_to_halfedges( *v_it );
    }

    {
        newMesh     = topologicalMesh.toTriangleMesh();
        bool check1 = true;
        bool check2 = true;
        for ( auto n : newMesh.normals() )
        {
            if ( !Ra::Core::Math::areApproxEqual(
                     n.dot( Vector3( Scalar( 1. ), Scalar( 0. ), Scalar( 0. ) ) ), Scalar( 1. ) ) )
            { check1 = false; }
            if ( n.dot( Vector3( Scalar( 0.5 ), Scalar( 0. ), Scalar( 0. ) ) ) > Scalar( 0.8 ) )
            { check2 = false; }
        }
        REQUIRE( check1 );
        REQUIRE( check2 );
    }
}
