#ifndef RADIUM_CONVERT_TESTS_HPP_
#define RADIUM_CONVERT_TESTS_HPP_

#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Tests/Tests.hpp>

using Ra::Core::TriangleMesh;
using Ra::Core::Vector3;

namespace RaTests {
/// Tests for TriangleMesh management.
class MeshTests : public Test {

    void testAttributeManagement() {
        using Vec3AttribHandle = Ra::Core::AttribHandle<Vector3>;

        TriangleMesh mesh = Ra::Core::MeshUtils::makeBox();

        // cannot add/access "in_position" or "in_normal"
        auto h_pos = mesh.addAttrib<Vector3>( "in_position" );
        RA_UNIT_TEST( !mesh.isValid( h_pos ), "Should be an invalid handle." );
        h_pos = mesh.getAttribHandle<Vector3>( "in_position" );
        RA_UNIT_TEST( !mesh.isValid( h_pos ), "Should be an invalid handle." );
        auto h_nor = mesh.addAttrib<Vector3>( "in_normal" );
        RA_UNIT_TEST( !mesh.isValid( h_nor ), "Should be an invalid handle." );
        h_nor = mesh.getAttribHandle<Vector3>( "in_normal" );
        RA_UNIT_TEST( !mesh.isValid( h_nor ), "Should be an invalid handle." );

        // Add/Remove attributes without filling it
        auto handlerEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
        mesh.removeAttrib( handlerEmpty );
        RA_UNIT_TEST( !mesh.isValid( handlerEmpty ), "Should be an invalid handle." );
        handlerEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
        RA_UNIT_TEST( mesh.isValid( handlerEmpty ), "Should get a valid handle here !" );
        mesh.removeAttrib( handlerEmpty );
        handlerEmpty = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "empty" );
        RA_UNIT_TEST( !mesh.isValid( handlerEmpty ), "Should be an invalid handle." );

        // Test access to the attribute container
        auto handlerFilled = mesh.addAttrib<Vec3AttribHandle::value_type>( "filled" );
        auto& container = mesh.getAttrib( handlerFilled ).data();
        auto containerHandler = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "filled" );
        auto& container2 = mesh.getAttrib( containerHandler ).data();
        RA_UNIT_TEST( container == container2, "getAttrib variants are not consistents" );

        // Test filling and removing vec3 attributes
        for ( int i = 0; i != mesh.vertices().size(); ++i )
            container.push_back( Vec3AttribHandle::value_type::Random() );
        mesh.removeAttrib( handlerFilled );

        // Test attribute creation by type, filling and removal
        auto handler = mesh.addAttrib<Eigen::Matrix<unsigned int, 1, 1>>( "filled2" );
        auto& container3 = mesh.getAttrib( handler ).data();
        using HandlerType = decltype( handler );

        for ( int i = 0; i != mesh.vertices().size(); ++i )
            container3.push_back( typename HandlerType::value_type( i ) );
        mesh.removeAttrib( handler );

        // Test dummy handler
        auto invalid = mesh.getAttribHandle<float>( "toto" );
        RA_UNIT_TEST( !mesh.isValid( invalid ), "Invalid Attrib Handle cannot be recognized" );

        // Test attribute copy
        const auto v0 = mesh.vertices()[0];
        TriangleMesh meshCopy = mesh;
        meshCopy.copyAllAttributes( mesh );
        RA_UNIT_TEST( mesh.vertices()[0].isApprox( v0 ), "Cannot copy TriangleMesh" );
        meshCopy.vertices()[0] += Ra::Core::Vector3( 0.5, 0.5, 0.5 );
        RA_UNIT_TEST( !meshCopy.vertices()[0].isApprox( v0 ),
                      "Cannot copy TriangleMesh attributes" );
    }

    void run() override { testAttributeManagement(); }
};

RA_TEST_CLASS( MeshTests )

} // namespace RaTests

#endif // RADIUM_CONVERT_TESTS_HPP_
