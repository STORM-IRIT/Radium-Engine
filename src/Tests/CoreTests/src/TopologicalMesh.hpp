#ifndef RADIUM_CONVERT_TESTS_HPP_
#define RADIUM_CONVERT_TESTS_HPP_

#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>
#include <Tests/Tests.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/LoopT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>

using Ra::Core::MeshConverter;
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
        Ra::Core::MeshConverter::convert( mesh, topologicalMesh );
        Ra::Core::MeshConverter::convert( topologicalMesh, newMesh );
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ), "Conversion to topological box mesh failed" );

        // Test for mesh with boundaries
        mesh = Ra::Core::MeshUtils::makePlaneGrid( 2, 2 );
        Ra::Core::MeshConverter::convert( mesh, topologicalMesh );
        Ra::Core::MeshConverter::convert( topologicalMesh, newMesh );
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ), "Conversion to topological grid mesh failed" );

        mesh = Ra::Core::MeshUtils::makeCylinder( Vector3( 0, 0, 0 ), Vector3( 0, 0, 1 ), 1 );
        Ra::Core::MeshConverter::convert( mesh, topologicalMesh );
        Ra::Core::MeshConverter::convert( topologicalMesh, newMesh );
        RA_UNIT_TEST( isSameMesh( mesh, newMesh ),
                      "Conversion to topological cylinder mesh failed" );
    }

    template <typename SubdividerType>
    void testSubdivision() {
        TriangleMesh mesh;
        TriangleMesh newMesh;
        TopologicalMesh topologicalMesh;

        // Generate input geometry
        const int nbIter = 2;
        const int gridSize = 1;

        mesh = Ra::Core::MeshUtils::makePlaneGrid( gridSize, gridSize );
        Ra::Core::MeshConverter::convert( mesh, topologicalMesh );

        // Subdivide
        SubdividerType subdivider;
        subdivider.attach( topologicalMesh );
        subdivider( nbIter );
        subdivider.detach();
        topologicalMesh.triangulate();
        Ra::Core::MeshConverter::convert( topologicalMesh, newMesh );

        RA_UNIT_TEST( newMesh.m_triangles.size() > 2 * nbIter * mesh.m_triangles.size(),
                      "Conversion to topological cylinder mesh failed" );
    }

    template <typename DecimationModule>
    void testDecimation() {

        TriangleMesh mesh;
        TriangleMesh newMesh;
        TopologicalMesh topologicalMesh;

        // Generate input geometry
        const int nbIter = 2;
        const int gridSize = 5;

        mesh = Ra::Core::MeshUtils::makePlaneGrid( gridSize, gridSize );
        Ra::Core::MeshConverter::convert( mesh, topologicalMesh );

        // Decimate
        Decimater decimater( topologicalMesh );
        DecimationModule mod;
        decimater.add( mod );
        decimater.initialize();
        decimater.decimate();
        Ra::Core::MeshConverter::convert( topologicalMesh, newMesh );

        RA_UNIT_TEST( newMesh.m_triangles.size() < 2 * nbIter * mesh.m_triangles.size(),
                      "Conversion to topological cylinder mesh failed" );
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

        i = 0;
        while ( result && i < meshOne.m_triangles.size() )
        {
            std::vector<Ra::Core::Vector3>::iterator it;
            stackVertices.clear();
            stackVertices.push_back( meshOne.vertices()[meshOne.m_triangles[i][0]] );
            stackVertices.push_back( meshOne.vertices()[meshOne.m_triangles[i][1]] );
            stackVertices.push_back( meshOne.vertices()[meshOne.m_triangles[i][2]] );
            for ( int j = 0; j < 3; ++j )
            {
                it = find( stackVertices.begin(), stackVertices.end(),
                           meshTwo.vertices()[meshTwo.m_triangles[i][j]] );
                if ( it != stackVertices.end() )
                {
                    stackVertices.erase( it );
                }
                else
                { result = false; }
            }
            ++i;
        }
        return result;
    }

    void run() override {
        testCopyConsistency();
        testSubdivision<Catmull>();
        testSubdivision<Loop>();
        testDecimation<HModQuadric>();
    }
};
RA_TEST_CLASS( TopologicalMeshTests );
} // namespace RaTests

#endif // RADIUM_CONVERT_TESTS_HPP_
