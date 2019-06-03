#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Tests.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
namespace Ra {
namespace Testing {

bool isSameMesh( Ra::Core::Geometry::TriangleMesh& meshOne,
                 Ra::Core::Geometry::TriangleMesh& meshTwo ) {

    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TopologicalMesh;
    using Ra::Core::Geometry::TriangleMesh;

    bool result = true;
    int i = 0;
    // Check length
    if ( meshOne.vertices().size() != meshTwo.vertices().size() )
        return false;
    if ( meshOne.normals().size() != meshTwo.normals().size() )
        return false;
    if ( meshOne.m_triangles.size() != meshTwo.m_triangles.size() )
        return false;

    // Check triangles
    std::vector<Ra::Core::Vector3> stackVertices;
    std::vector<Ra::Core::Vector3> stackNormals;

    i = 0;
    while ( result && i < meshOne.m_triangles.size() )
    {
        std::vector<Ra::Core::Vector3>::iterator it;
        stackVertices.clear();
        stackVertices.push_back( meshOne.vertices()[meshOne.m_triangles[i][0]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.m_triangles[i][1]] );
        stackVertices.push_back( meshOne.vertices()[meshOne.m_triangles[i][2]] );

        stackNormals.clear();
        stackNormals.push_back( meshOne.normals()[meshOne.m_triangles[i][0]] );
        stackNormals.push_back( meshOne.normals()[meshOne.m_triangles[i][1]] );
        stackNormals.push_back( meshOne.normals()[meshOne.m_triangles[i][2]] );

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackVertices.begin(), stackVertices.end(),
                       meshTwo.vertices()[meshTwo.m_triangles[i][j]] );
            if ( it != stackVertices.end() )
            {
                stackVertices.erase( it );
            } else
            { result = false; }
        }

        for ( int j = 0; j < 3; ++j )
        {
            it = find( stackNormals.begin(), stackNormals.end(),
                       meshTwo.normals()[meshTwo.m_triangles[i][j]] );
            if ( it != stackNormals.end() )
            {
                stackNormals.erase( it );
            } else
            { result = false; }
        }

        ++i;
    }
    return result;
}

void run() {
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
    mesh = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh = topologicalMesh.toTriangleMesh();
    RA_VERIFY( isSameMesh( mesh, newMesh ), "Conversion to topological box mesh" );

    mesh = Ra::Core::Geometry::makeSharpBox();
    topologicalMesh = TopologicalMesh( mesh );
    newMesh = topologicalMesh.toTriangleMesh();
    RA_VERIFY( isSameMesh( mesh, newMesh ), "Conversion to topological sharp box mesh" );

    // Test for mesh with boundaries
    mesh = Ra::Core::Geometry::makePlaneGrid( 2, 2 );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh = topologicalMesh.toTriangleMesh();
    RA_VERIFY( isSameMesh( mesh, newMesh ), "Conversion to topological grid mesh" );

    mesh = Ra::Core::Geometry::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh = topologicalMesh.toTriangleMesh();
    RA_VERIFY( isSameMesh( mesh, newMesh ), "Conversion to topological cylinder mesh" );

    // Test skip empty attributes
    mesh.addAttrib<float>( "empty" );
    topologicalMesh = TopologicalMesh( mesh );
    newMesh = topologicalMesh.toTriangleMesh();
    RA_VERIFY( !newMesh.hasAttrib( "empty" ), "TopologicalMesh skip empty attributes" );

    // Test normals
    mesh = Ra::Core::Geometry::makeBox();
    topologicalMesh = TopologicalMesh( mesh );

    for ( TopologicalMesh::ConstVertexIter v_it = topologicalMesh.vertices_begin();
          v_it != topologicalMesh.vertices_end(); ++v_it )
    {
        topologicalMesh.set_normal(
            *v_it, TopologicalMesh::Normal( Scalar( 1. ), Scalar( 0. ), Scalar( 0. ) ) );
    }

    for ( TopologicalMesh::ConstVertexIter v_it = topologicalMesh.vertices_begin();
          v_it != topologicalMesh.vertices_end(); ++v_it )
    {
        topologicalMesh.propagate_normal_to_halfedges( *v_it );
    }

    {
        newMesh = topologicalMesh.toTriangleMesh();
        bool check1 = true;
        bool check2 = true;
        for ( auto n : newMesh.normals() )
        {
            if ( !Ra::Core::Math::areApproxEqual(
                     n.dot( Vector3( Scalar( 1. ), Scalar( 0. ), Scalar( 0. ) ) ), Scalar( 1. ) ) )
            {
                check1 = false;
            }
            if ( n.dot( Vector3( Scalar( 0.5 ), Scalar( 0. ), Scalar( 0. ) ) ) > Scalar( 0.8 ) )
            {
                check2 = false;
            }
        }
        RA_VERIFY( check1 && check2, "Set normal to topo vertex and apply them to halfedge" );
    }
}
} // namespace Testing
} // namespace Ra

int main( int argc, const char** argv ) {
    using namespace Ra;

    if ( !Testing::init_testing( 1, argv ) )
    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
    for ( int i = 0; i < Testing::g_repeat; ++i )
    {
        CALL_SUBTEST( ( Testing::run() ) );
    }

    return EXIT_SUCCESS;
}
