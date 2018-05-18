#ifndef RADIUM_CONVERT_TESTS_HPP_
#define RADIUM_CONVERT_TESTS_HPP_

#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Tests/Tests.hpp>

using Ra::Core::TriangleMesh;
using Ra::Core::Vector3;

namespace RaTests {

class MeshTests : public Test {

    void testAttributeManagement() {
        using Vec3AttribHandle = Ra::Core::AttribHandle<Vector3>;

        TriangleMesh mesh;

        // Add/Remove attributes without filling it
        mesh = Ra::Core::MeshUtils::makeBox();
        mesh.attribManager().addAttrib<Vec3AttribHandle::value_type>( "empty" );
        mesh.attribManager().removeAttrib( "empty" );
        auto handlerEmpty = mesh.attribManager().addAttrib<Vec3AttribHandle::value_type>( "empty" );
        RA_UNIT_TEST( handlerEmpty.isValid(), "Should get a valid handler here !" );
        mesh.attribManager().removeAttrib( handlerEmpty );

        // Test access to the attribute container
        auto handlerFilled =
            mesh.attribManager().addAttrib<Vec3AttribHandle::value_type>( "filled" );
        auto& container = mesh.attribManager().getAttrib( handlerFilled ).data();
        auto containerHandler =
            mesh.attribManager().getAttribHandler<Vec3AttribHandle::value_type>( "filled" );
        auto& container2 = mesh.attribManager().getAttrib( containerHandler ).data();
        RA_UNIT_TEST( container == container2, "getAttrib variants are not consistents" );

        // Test filling and removing vec3 attributes
        for ( int i = 0; i != mesh.vertices().size(); ++i )
            container.push_back( Vec3AttribHandle::value_type::Random() );
        mesh.attribManager().removeAttrib( "filled" );

        // Test attribute creation by type, filling and removal
        auto handler =
            mesh.attribManager().addAttrib<Eigen::Matrix<unsigned int, 1, 1>>( "filled2" );
        auto& container3 = mesh.attribManager().getAttrib( handler ).data();
        using HandlerType = decltype( handler );

        for ( int i = 0; i != mesh.vertices().size(); ++i )
            container3.push_back( typename HandlerType::value_type( i ) );
        mesh.attribManager().removeAttrib( "filled2" );

        // Test dummy handler
        auto invalid = mesh.attribManager().getAttribHandler<float>( "toto" );
        RA_UNIT_TEST( !invalid.isValid(), "Invalid Attrib Handler cannot be recognized" );
    }

    void run() override { testAttributeManagement(); }
};
RA_TEST_CLASS( MeshTests );
} // namespace RaTests

#endif // RADIUM_CONVERT_TESTS_HPP_
