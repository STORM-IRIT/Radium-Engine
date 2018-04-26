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
        using Vec3AttribHandle = Ra::Core::VertexAttribHandle<Vector3>;

        TriangleMesh mesh;

        // Add/Remove attributes without filling it
        mesh = Ra::Core::MeshUtils::makeBox();
        Vec3AttribHandle handlerEmpty, handlerFilled;
        mesh.attribManager().addAttrib( handlerEmpty, "empty" );
        mesh.attribManager().removeAttrib( "empty" );

        // Test access to the attribute container
        mesh.attribManager().addAttrib( handlerFilled, "filled" );
        auto& container = mesh.attribManager().getAttrib( handlerFilled ).data();
        auto& container2 =
            mesh.attribManager().getAttrib( "filled" ).cast<Vec3AttribHandle::value_type>().data();
        RA_UNIT_TEST( container == container2, "getAttrib variants are not consistents" );

        // Test filling and removing vec3 attributes
        for ( int i = 0; i != mesh.vertices().size(); ++i )
            container.push_back( Vec3AttribHandle::value_type::Random() );
        mesh.attribManager().removeAttrib( "filled" );

        // Test attribute creation by type, filling and removal
        Eigen::Matrix<unsigned int, 1, 1> proxy;
        auto handler = mesh.attribManager().addAttrib( proxy, "filled2" );
        auto& container3 = mesh.attribManager().getAttrib( handler ).data();
        using HandlerType = decltype( handler );

        for ( int i = 0; i != mesh.vertices().size(); ++i )
            container3.push_back( typename HandlerType::value_type( i ) );
        mesh.attribManager().removeAttrib( "filled2" );

        // Test dummy handler
        auto& dummy = mesh.attribManager().getAttrib( "toto" );
        RA_UNIT_TEST( dummy == mesh.attribManager().getDummyAttrib(),
                      "Dummy Attrib Handler cannot be recognized" );
    }

    void run() override { testAttributeManagement(); }
};
RA_TEST_CLASS( MeshTests );
} // namespace RaTests

#endif // RADIUM_CONVERT_TESTS_HPP_
