#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Geometry/TriangleMesh", "[Core][Core/Geometry][TriangleMesh]" ) {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TriangleMesh;
    using Vec3AttribHandle = Ra::Core::Utils::AttribHandle<Vector3>;

    TriangleMesh mesh = Ra::Core::Geometry::makeBox();

    // cannot add/access "in_position" or "in_normal"
    auto h_pos = mesh.addAttrib<Vector3>( "in_position" );
    REQUIRE( !mesh.isValid( h_pos ) );
    h_pos = mesh.getAttribHandle<Vector3>( "in_position" );
    REQUIRE( !mesh.isValid( h_pos ) );
    auto h_nor = mesh.addAttrib<Vector3>( "in_normal" );
    REQUIRE( !mesh.isValid( h_nor ) );
    h_nor = mesh.getAttribHandle<Vector3>( "in_normal" );
    REQUIRE( !mesh.isValid( h_nor ) );

    // Add/Remove attributes without filling it
    auto handlerEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    mesh.removeAttrib( handlerEmpty );
    REQUIRE( !mesh.isValid( handlerEmpty ) );
    handlerEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    REQUIRE( mesh.isValid( handlerEmpty ) );
    mesh.removeAttrib( handlerEmpty );
    handlerEmpty = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "empty" );
    REQUIRE( !mesh.isValid( handlerEmpty ) );

    // Test access to the attribute container
    auto handlerFilled    = mesh.addAttrib<Vec3AttribHandle::value_type>( "filled" );
    auto& container       = mesh.getAttrib( handlerFilled ).data();
    auto containerHandler = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "filled" );
    auto& container2      = mesh.getAttrib( containerHandler ).data();
    REQUIRE( container == container2 );

    // Test filling and removing vec3 attributes
    for ( int i = 0; i != int( mesh.vertices().size() ); ++i )
        container.push_back( Vec3AttribHandle::value_type::Random() );
    mesh.removeAttrib( handlerFilled );

    // Test attribute creation by type, filling and removal
    auto handler      = mesh.addAttrib<Eigen::Matrix<unsigned int, 1, 1>>( "filled2" );
    auto& container3  = mesh.getAttrib( handler ).data();
    using HandlerType = decltype( handler );

    for ( int i = 0; i != int( mesh.vertices().size() ); ++i )
        container3.push_back( typename HandlerType::value_type( i ) );
    mesh.removeAttrib( handler );

    // Test dummy handler
    auto invalid = mesh.getAttribHandle<float>( "toto" );
    REQUIRE( !mesh.isValid( invalid ) );

    // Test attribute copy
    const auto v0         = mesh.vertices()[0];
    TriangleMesh meshCopy = mesh;
    meshCopy.copyAllAttributes( mesh );
    REQUIRE( mesh.vertices()[0].isApprox( v0 ) );
    meshCopy.vertices()[0] += Ra::Core::Vector3( 0.5, 0.5, 0.5 );
    REQUIRE( !meshCopy.vertices()[0].isApprox( v0 ) );
}
