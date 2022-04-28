#include <Core/Asset/GeometryData.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Asset/GeometryData", "[Core][Core/Asset][GeometryData]" ) {

    using namespace Ra::Core::Asset;
    using namespace Ra::Core::Geometry;

    SECTION( "Normal test" ) {
        auto geometry = new GeometryData();
        auto& normal  = geometry->getNormals();
        REQUIRE( !geometry->hasNormals() );
        REQUIRE(
            geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_NORMAL )->isLocked() );

        normal.resize( 1, Ra::Core::Vector3::Zero() );
        normal[0] = Ra::Core::Vector3().setRandom();
        auto save = normal[0];
        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_NORMAL )->unlock();
        REQUIRE( geometry->hasNormals() );

        auto& name        = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_NORMAL );
        auto attriHandler = geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( name );
        const auto& data  = geometry->getAttribManager().getAttrib( attriHandler ).data();
        REQUIRE( save == data[0] );

        auto geometry2 = new GeometryData();
        geometry2->setNormals( normal );
        auto& name2        = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_NORMAL );
        auto attriHandler2 = geometry2->getAttribManager().findAttrib<Ra::Core::Vector3>( name2 );
        const auto& attrib = geometry2->getAttribManager().getAttrib( attriHandler2 );
        REQUIRE( attrib.data().size() == 1 );
        REQUIRE( attrib.data()[0] == save );
    }

    SECTION( "Vertices test" ) {
        auto geometry = new GeometryData();
        auto& vertex  = geometry->getVertices();
        REQUIRE( !geometry->hasVertices() );
        REQUIRE(
            geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_POSITION )->isLocked() );

        vertex.resize( 1, Ra::Core::Vector3::Zero() );
        vertex[0] = Ra::Core::Vector3().setRandom();
        auto save = vertex[0];
        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_POSITION )->unlock();
        REQUIRE( geometry->hasVertices() );

        auto& name        = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_POSITION );
        auto attriHandler = geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( name );
        const auto& data  = geometry->getAttribManager().getAttrib( attriHandler ).data();
        REQUIRE( save == data[0] );

        auto geometry2 = new GeometryData();
        geometry2->setVertices( vertex );
        auto& name2        = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_POSITION );
        auto attriHandler2 = geometry2->getAttribManager().findAttrib<Ra::Core::Vector3>( name2 );
        const auto& attrib = geometry2->getAttribManager().getAttrib( attriHandler2 );
        REQUIRE( attrib.data().size() == 1 );
        REQUIRE( attrib.data()[0] == save );
    }

    SECTION( "Tangent, BiTangent, TexCoord tests" ) {
        auto geometry = new GeometryData();

        // Tangent test
        auto& tangents = geometry->getTangents();
        REQUIRE( !geometry->hasTangents() );
        REQUIRE(
            geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_TANGENT )->isLocked() );

        tangents.resize( 1, Ra::Core::Vector3::Zero() );
        tangents[0]  = Ra::Core::Vector3().setRandom();
        auto saveTan = tangents[0];
        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_TANGENT )->unlock();
        REQUIRE( geometry->hasTangents() );

        auto& nameTan = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_TANGENT );
        auto attribHandlerTan =
            geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( nameTan );
        const auto& dataTan = geometry->getAttribManager().getAttrib( attribHandlerTan ).data();
        REQUIRE( saveTan == dataTan[0] );

        auto geoTanTest = new GeometryData();
        geoTanTest->setTangents( tangents );
        auto attribHandlerTan2 =
            geoTanTest->getAttribManager().findAttrib<Ra::Core::Vector3>( nameTan );
        const auto& attribTan = geoTanTest->getAttribManager().getAttrib( attribHandlerTan2 );
        REQUIRE( attribTan.data().size() == 1 );
        REQUIRE( attribTan.data()[0] == saveTan );

        // BiTangent test
        auto& biTangents = geometry->getBiTangents();
        REQUIRE( !geometry->hasBiTangents() );
        REQUIRE( geometry->getAttribManager()
                     .getAttribBase( MeshAttrib::VERTEX_BITANGENT )
                     ->isLocked() );

        biTangents.resize( 1, Ra::Core::Vector3::Zero() );
        biTangents[0]  = Ra::Core::Vector3().setRandom();
        auto saveBiTan = biTangents[0];
        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_BITANGENT )->unlock();
        REQUIRE( geometry->hasBiTangents() );

        auto& nameBiTan = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_BITANGENT );
        auto attribHandlerBiTan =
            geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( nameBiTan );
        const auto& dataBiTan = geometry->getAttribManager().getAttrib( attribHandlerBiTan ).data();
        REQUIRE( saveBiTan == dataBiTan[0] );

        auto geoBiTanTest = new GeometryData();
        geoBiTanTest->setBitangents( biTangents );
        auto attribHandlerBiTan2 =
            geoBiTanTest->getAttribManager().findAttrib<Ra::Core::Vector3>( nameBiTan );
        const auto& attribBiTan = geoBiTanTest->getAttribManager().getAttrib( attribHandlerBiTan2 );

        REQUIRE( attribBiTan.data().size() == 1 );
        REQUIRE( attribBiTan.data()[0] == saveBiTan );

        // TexCoords test
        auto& texCoords = geometry->getTexCoords();
        REQUIRE( !geometry->hasTextureCoordinates() );
        REQUIRE(
            geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_TEXCOORD )->isLocked() );

        texCoords.resize( 1, Ra::Core::Vector3::Zero() );
        texCoords[0]       = Ra::Core::Vector3().setRandom();
        auto saveTexCoords = texCoords[0];
        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_TEXCOORD )->unlock();
        REQUIRE( geometry->hasTextureCoordinates() );

        auto& nameTexCoords = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_TEXCOORD );
        auto attribHandlerTexCoords =
            geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( nameTexCoords );
        const auto& dataTexCoords =
            geometry->getAttribManager().getAttrib( attribHandlerTexCoords ).data();
        REQUIRE( saveTexCoords == dataTexCoords[0] );

        auto geoTexCoordsTest = new GeometryData();
        geoTexCoordsTest->setTextureCoordinates( texCoords );
        auto attribHandlerTexCoords2 =
            geoTexCoordsTest->getAttribManager().findAttrib<Ra::Core::Vector3>( nameTexCoords );
        const auto& attribTexCoords =
            geoTexCoordsTest->getAttribManager().getAttrib( attribHandlerTexCoords2 );
        REQUIRE( attribTexCoords.data().size() == 1 );
        REQUIRE( attribTexCoords.data()[0] == saveTexCoords );
    }

    SECTION( "Edges test " ) {
        auto geometry = GeometryData();
        auto& edges   = geometry.getEdges();
        REQUIRE( !geometry.hasEdges() );

        edges.resize( 1 );
        edges[0]  = Ra::Core::Vector2ui().setRandom();
        auto save = edges[0];
        geometry.indexedDataUnlock( GeometryData::GeometryType::LINE_MESH, "in_edge" );
        REQUIRE( geometry.hasEdges() );

        const auto& data = geometry.getIndexedData<Ra::Core::Vector2ui>(
            GeometryData::GeometryType::LINE_MESH, "in_edge" );
        REQUIRE( save == data[0] );

        auto geometry2 = GeometryData();
        geometry2.setEdges( edges );
        // geometry2.setIndexedData(GeometryData::GeometryType::LINE_MESH, edges, "in_edge");
        REQUIRE( geometry2.getEdges().size() == 1 );
        REQUIRE( geometry.getEdges()[0] == save );
    }

    SECTION( "Faces test " ) {
        auto geometry = GeometryData();
        auto& faces   = geometry.getFaces();
        REQUIRE( !geometry.hasFaces() );

        faces.resize( 1 );
        faces[0]  = Ra::Core::VectorNui().setRandom();
        auto save = faces[0];
        geometry.indexedDataUnlock( GeometryData::GeometryType::POLY_MESH, "in_face" );
        REQUIRE( geometry.hasFaces() );

        const auto& data = geometry.getIndexedData<Ra::Core::VectorNui>(
            GeometryData::GeometryType::POLY_MESH, "in_face" );
        REQUIRE( save == data[0] );

        auto geometry2 = GeometryData();
        geometry2.setFaces( faces );
        // geometry2.setIndexedData(GeometryData::GeometryType::POLY_MESH, faces, "in_face");
        REQUIRE( geometry2.getFaces().size() == 1 );
        REQUIRE( geometry.getFaces()[0] == save );
    }

    SECTION( "Polyhedron test " ) {
        auto geometry     = GeometryData();
        auto& polyhedrons = geometry.getPolyhedra();
        REQUIRE( !geometry.hasPolyhedra() );

        polyhedrons.resize( 1 );
        polyhedrons[0] = Ra::Core::VectorNui().setRandom();
        auto save      = polyhedrons[0];
        geometry.indexedDataUnlock( GeometryData::GeometryType::POLY_MESH, "in_polyhedron" );
        REQUIRE( geometry.hasPolyhedra() );

        const auto& data = geometry.getIndexedData<Ra::Core::VectorNui>(
            GeometryData::GeometryType::POLY_MESH, "in_polyhedron" );
        REQUIRE( save == data[0] );

        auto geometry2 = GeometryData();
        geometry2.setPolyhedra( polyhedrons );
        // geometry2.setIndexedData(GeometryData::GeometryType::POLY_MESH, polyhedrons,
        // "in_polyhedron");
        REQUIRE( geometry2.getPolyhedra().size() == 1 );
        REQUIRE( geometry.getPolyhedra()[0] == save );
    }

    SECTION( "Type test " ) {
        auto geometry = GeometryData();

        geometry.setType( GeometryData::GeometryType::LINE_MESH );
        REQUIRE( geometry.isLineMesh() );

        geometry.setType( GeometryData::GeometryType::TRI_MESH );
        REQUIRE( geometry.isTriMesh() );

        geometry.setType( GeometryData::GeometryType::QUAD_MESH );
        REQUIRE( geometry.isQuadMesh() );

        geometry.setType( GeometryData::GeometryType::POLY_MESH );
        REQUIRE( geometry.isPolyMesh() );

        geometry.setType( GeometryData::GeometryType::HEX_MESH );
        REQUIRE( geometry.isHexMesh() );

        geometry.setType( GeometryData::GeometryType::TETRA_MESH );
        REQUIRE( geometry.isTetraMesh() );

        geometry.setType( GeometryData::GeometryType::POINT_CLOUD );
        REQUIRE( geometry.isPointCloud() );

        geometry.setType( GeometryData::GeometryType::UNKNOWN );
        REQUIRE( !geometry.isLineMesh() );
        REQUIRE( !geometry.isTriMesh() );
        REQUIRE( !geometry.isQuadMesh() );
        REQUIRE( !geometry.isPolyMesh() );
        REQUIRE( !geometry.isHexMesh() );
        REQUIRE( !geometry.isTetraMesh() );
        REQUIRE( !geometry.isPointCloud() );
    }
}
