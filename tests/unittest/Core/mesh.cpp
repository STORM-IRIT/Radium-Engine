#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Geometry/TriangleMesh", "[Core][Core/Geometry][TriangleMesh]" ) {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::TriangleMesh;
    using Vec3AttribHandle = Ra::Core::Utils::AttribHandle<Vector3>;

    TriangleMesh mesh = Ra::Core::Geometry::makeBox();

    // base attributes are automatically added
    auto h_pos = mesh.getAttribHandle<Vector3>( "in_position" );
    REQUIRE( mesh.isValid( h_pos ) );
    auto h_nor = mesh.getAttribHandle<Vector3>( "in_normal" );
    REQUIRE( mesh.isValid( h_nor ) );

    // Add/Remove attributes without filling it
    auto handleEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    mesh.removeAttrib( handleEmpty );
    REQUIRE( !mesh.isValid( handleEmpty ) );
    handleEmpty = mesh.addAttrib<Vec3AttribHandle::value_type>( "empty" );
    REQUIRE( mesh.isValid( handleEmpty ) );
    mesh.removeAttrib( handleEmpty );
    handleEmpty = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "empty" );
    REQUIRE( !mesh.isValid( handleEmpty ) );

    // Test access to the attribute container
    auto handleFilled     = mesh.addAttrib<Vec3AttribHandle::value_type>( "filled" );
    auto& attribFilled    = mesh.getAttrib( handleFilled );
    auto& containerFilled = attribFilled.getDataWithLock();
    REQUIRE( attribFilled.isLocked() );

    // Test filling and removing vec3 attributes
    for ( int i = 0; i != int( mesh.vertices().size() ); ++i )
        containerFilled.push_back( Vec3AttribHandle::value_type::Random() );
    attribFilled.unlock();

    auto handleFilled2     = mesh.getAttribHandle<Vec3AttribHandle::value_type>( "filled" );
    auto& containerFilled2 = mesh.getAttrib( handleFilled2 ).data();
    REQUIRE( containerFilled == containerFilled2 );

    mesh.removeAttrib( handleFilled );

    // Test attribute creation by type, filling and removal
    auto handle      = mesh.addAttrib<Eigen::Matrix<unsigned int, 1, 1>>( "filled2" );
    auto& container3 = mesh.getAttrib( handle ).getDataWithLock();
    using HandleType = decltype( handle );

    for ( int i = 0; i != int( mesh.vertices().size() ); ++i )
        container3.push_back( typename HandleType::value_type( i ) );
    mesh.getAttrib( handle ).unlock();
    mesh.removeAttrib( handle );

    // Test dummy handle
    auto invalid = mesh.getAttribHandle<float>( "toto" );
    REQUIRE( !mesh.isValid( invalid ) );

    // Test attribute copy
    const auto v0         = mesh.vertices()[0];
    TriangleMesh meshCopy = mesh;
    meshCopy.copyAllAttributes( mesh );
    REQUIRE( mesh.vertices()[0].isApprox( v0 ) );
    meshCopy.verticesWithLock()[0] += Ra::Core::Vector3( 0.5, 0.5, 0.5 );
    meshCopy.verticesUnlock();
    REQUIRE( !meshCopy.vertices()[0].isApprox( v0 ) );

    // For the documentation in doc/developer/mesh.md
    {
        TriangleMesh m;
        TriangleMesh::PointAttribHandle::Container vertices;
        TriangleMesh::NormalAttribHandle::Container normals;
        TriangleMesh::IndexContainerType indices;

        vertices.push_back( {0, 0, 0} );
        vertices.push_back( {1, 0, 0} );
        vertices.push_back( {0, 2, 0} );
        normals.push_back( {0, 0, 1} );
        normals.push_back( {0, 0, 1} );
        normals.push_back( {0, 0, 1} );

        m.setVertices( std::move( vertices ) );
        m.setNormals( std::move( normals ) );

        m.setIndices( {{0, 1, 2}} );

        auto handle1  = m.addAttrib<Vector3>( "vector3_attrib" );
        auto& attrib1 = m.getAttrib( handle1 );
        auto& buf     = attrib1.getDataWithLock();

        buf.reserve( 3 );
        buf.push_back( {1, 1, 1} );
        buf.push_back( {2, 2, 2} );
        buf.push_back( {3, 3, 3} );
        attrib1.unlock();

        auto handle2  = m.addAttrib<float>( "float_attrib" );
        auto& attrib2 = m.getAttrib( handle2 );
        attrib2.setData( {1.f, 2.f, 3.f} );

        TriangleMesh m2;
        m2.copyBaseGeometry( m );
        m2.copyAttributes( m, handle1 );
    }
}
