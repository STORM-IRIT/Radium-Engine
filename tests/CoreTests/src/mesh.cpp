#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {

void run()  {
    using Ra::Core::TriangleMesh;
    using Ra::Core::Vector3;
    using Vec3AttribHandle = Ra::Core::Utils::AttribHandle<Vector3>;

    TriangleMesh mesh = Ra::Core::MeshUtils::makeBox();

    // cannot add/access "in_position" or "in_normal"
    auto h_pos = mesh.addAttrib<Vector3>( "in_position" );
    RA_VERIFY( !mesh.isValid( h_pos ), "Should be an invalid handle." );
    h_pos = mesh.getAttribHandle<Vector3>( "in_position" );
    RA_VERIFY( !mesh.isValid( h_pos ), "Should be an invalid handle." );
    auto h_nor = mesh.addAttrib<Vector3>( "in_normal" );
    RA_VERIFY( !mesh.isValid( h_nor ), "Should be an invalid handle." );
    h_nor = mesh.getAttribHandle<Vector3>( "in_normal" );
    RA_VERIFY( !mesh.isValid( h_nor ), "Should be an invalid handle." );

    // Add/Remove attributes without filling it
    auto handlerEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    mesh.removeAttrib( handlerEmpty );
    RA_VERIFY( !mesh.isValid( handlerEmpty ), "Should be an invalid handle." );
    handlerEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    RA_VERIFY( mesh.isValid( handlerEmpty ), "Should get a valid handle here !" );
    mesh.removeAttrib( handlerEmpty );
    handlerEmpty = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "empty" );
    RA_VERIFY( !mesh.isValid( handlerEmpty ), "Should be an invalid handle." );

    // Test access to the attribute container
    auto handlerFilled = mesh.addAttrib<Vec3AttribHandle::value_type>( "filled" );
    auto& container = mesh.getAttrib( handlerFilled ).data();
    auto containerHandler = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "filled" );
    auto& container2 = mesh.getAttrib( containerHandler ).data();
    RA_VERIFY( container == container2, "getAttrib variants are not consistents" );

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
    RA_VERIFY( !mesh.isValid( invalid ), "Invalid Attrib Handle cannot be recognized" );

    // Test attribute copy
    const auto v0 = mesh.vertices()[0];
    TriangleMesh meshCopy = mesh;
    meshCopy.copyAllAttributes( mesh );
    RA_VERIFY( mesh.vertices()[0].isApprox( v0 ), "Cannot copy TriangleMesh" );
    meshCopy.vertices()[0] += Ra::Core::Vector3( 0.5, 0.5, 0.5 );
    RA_VERIFY( !meshCopy.vertices()[0].isApprox( v0 ),
                  "Cannot copy TriangleMesh attributes" );
}
} // namespace Testing
} // namespace Ra


int main(int argc, const char **argv) {
    using namespace Ra;

    if(!Testing::init_testing(1, argv))
    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
    for(int i = 0; i < Testing::g_repeat; ++i)
    {
        CALL_SUBTEST(( Testing::run() ));
    }

    return EXIT_SUCCESS;
}

