#ifndef RADIUM_TOPOLOGICALMESH_TESTS_HPP_
#define RADIUM_TOPOLOGICALMESH_TESTS_HPP_

#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Tests/Tests.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>

using Ra::Core::TopologicalMesh;
using Ra::Core::TriangleMesh;
using Ra::Core::Vector3;

namespace RaTests {

class TopologicalMeshTests : public Test {
    using Catmull = OpenMesh::Subdivider::Uniform::CatmullClarkT<Ra::Core::TopologicalMesh>;
    using Loop = OpenMesh::Subdivider::Uniform::LoopT<Ra::Core::TopologicalMesh>;

    using Decimater = OpenMesh::Decimater::DecimaterT<Ra::Core::TopologicalMesh>;
    using HModQuadric = OpenMesh::Decimater::ModQuadricT<Ra::Core::TopologicalMesh>::Handle;

    void testCopyConsistency() {
        TriangleMesh newMesh;
        TriangleMesh mesh;
        TopologicalMesh topologicalMesh;

        // Test for close mesh
        mesh = Ra::Core::MeshUtils::makeBox();
        topologicalMesh = TopologicalMesh( mesh );
        newMesh = topologicalMesh.toTriangleMesh();
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ), "Conversion to topological box mesh" );

        mesh = Ra::Core::MeshUtils::makeSharpBox();
        topologicalMesh = TopologicalMesh( mesh );
        newMesh = topologicalMesh.toTriangleMesh();
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ), "Conversion to topological sharp box mesh" );

        // Test for mesh with boundaries
        mesh = Ra::Core::MeshUtils::makePlaneGrid( 2, 2 );
        topologicalMesh = TopologicalMesh( mesh );
        newMesh = topologicalMesh.toTriangleMesh();
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ), "Conversion to topological grid mesh" );

        mesh = Ra::Core::MeshUtils::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );
        topologicalMesh = TopologicalMesh( mesh );

        newMesh = topologicalMesh.toTriangleMesh();
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ), "Conversion to topological cylinder mesh" );
    }

    bool isSameMesh( TriangleMesh& meshOne, TriangleMesh& meshTwo ) {
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

    void run() override { testCopyConsistency(); }
};
RA_TEST_CLASS( TopologicalMeshTests );

} // namespace RaTests

#endif
