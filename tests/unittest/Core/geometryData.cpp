#include "Core/Geometry/StandardAttribNames.hpp"
#include <Core/Asset/GeometryData.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Asset/GeometryData", "[Core][Core/Asset][GeometryData]" ) {

    using namespace Ra::Core::Asset;
    using namespace Ra::Core::Geometry;

    SECTION( "Normal test" ) {
        auto geometry = new GeometryData();
        auto& normal  = geometry->getNormals();
        auto& name    = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_NORMAL );

        REQUIRE( normal.empty() );
        REQUIRE( geometry->getAttribManager().getAttribBase( name )->isLocked() );

        normal.resize( 1, Ra::Core::Vector3::Zero() );
        normal[0] = Ra::Core::Vector3().setRandom();
        auto save = normal[0];
        geometry->getAttribManager().getAttribBase( name )->unlock();
        REQUIRE( !normal.empty() );
        REQUIRE( !geometry->getAttribManager().getAttribBase( name )->isLocked() );

        auto attriHandler = geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( name );
        const auto& data  = geometry->getAttribManager().getAttrib( attriHandler ).data();
        REQUIRE( save == data[0] );

        auto geometry2 = new GeometryData();
        geometry2->setNormals( normal );
        auto attriHandler2 = geometry2->getAttribManager().findAttrib<Ra::Core::Vector3>( name );
        const auto& attrib = geometry2->getAttribManager().getAttrib( attriHandler2 );
        REQUIRE( attrib.data().size() == 1 );
        REQUIRE( attrib.data()[0] == save );
    }

    SECTION( "Vertices test" ) {
        auto geometry = new GeometryData();
        auto& vertex  = geometry->getVertices();
        REQUIRE( vertex.empty() );
        REQUIRE(
            geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_POSITION )->isLocked() );

        vertex.resize( 1, Ra::Core::Vector3::Zero() );
        vertex[0] = Ra::Core::Vector3().setRandom();
        auto save = vertex[0];
        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_POSITION )->unlock();
        REQUIRE( !vertex.empty() );
        REQUIRE( !geometry->getAttribManager()
                      .getAttribBase( MeshAttrib::VERTEX_POSITION )
                      ->isLocked() );

        auto& name       = Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_POSITION );
        auto attriHandle = geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( name );
        const auto& data = geometry->getAttribManager().getAttrib( attriHandle ).data();
        REQUIRE( save == data[0] );

        auto geometry2 = new GeometryData();
        geometry2->getMultiIndexedGeometry().setVertices(
            geometry->getMultiIndexedGeometry().vertices() );
        auto attriHandle2  = geometry2->getAttribManager().findAttrib<Ra::Core::Vector3>( name );
        const auto& attrib = geometry2->getAttribManager().getAttrib( attriHandle2 );
        REQUIRE( attrib.data().size() == 1 );
        REQUIRE( attrib.data()[0] == save );
    }

    SECTION( "Tangent, BiTangent, TexCoord tests" ) {
        auto geom               = new GeometryData();
        auto& tangentAttribName = getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TANGENT );
        auto tangentAttribHandle =
            geom->getMultiIndexedGeometry().addAttrib<Ra::Core::Vector3>( tangentAttribName );
        auto& tangents =
            geom->getMultiIndexedGeometry().getAttrib( tangentAttribHandle ).getDataWithLock();

        REQUIRE( tangents.empty() );
        REQUIRE( geom->getAttribManager().getAttribBase( tangentAttribName )->isLocked() );

        tangents.resize( 1, Ra::Core::Vector3::Zero() );
        tangents[0]  = Ra::Core::Vector3().setRandom();
        auto saveTan = tangents[0];
        geom->getAttribManager().getAttribBase( tangentAttribName )->unlock();

        REQUIRE( !tangents.empty() );
        REQUIRE( !geom->getAttribManager().getAttribBase( tangentAttribName )->isLocked() );

        auto tangentAttribHandle2 =
            geom->getAttribManager().findAttrib<Ra::Core::Vector3>( tangentAttribName );
        const auto& dataTan = geom->getAttribManager().getAttrib( tangentAttribHandle2 ).data();
        REQUIRE( saveTan == dataTan[0] );

        auto geom2 = new GeometryData();
        auto geom2TangentAttribHandle2 =
            geom2->getMultiIndexedGeometry().addAttrib<Ra::Core::Vector3>( tangentAttribName );
        geom2->getMultiIndexedGeometry()
            .getAttrib<Ra::Core::Vector3>( tangentAttribHandle2 )
            .setData( dataTan );

        auto tangentAttribHandle3 =
            geom2->getAttribManager().findAttrib<Ra::Core::Vector3>( tangentAttribName );

        const auto& tangents2 = geom2->getAttribManager().getAttrib( tangentAttribHandle3 );

        REQUIRE( tangents2.data().size() == 1 );
        REQUIRE( tangents2.data()[0] == saveTan );
        /// \todo to be continued
        /*

                // BiTangent test
                auto& biTangents = geometry->getBiTangents();
                auto& biTangentAttribName =
                    Ra::Core::Geometry::getAttribName( MeshAttrib::VERTEX_BITANGENT );
                REQUIRE( biTangents.empty() );
                REQUIRE( geometry->getAttribManager().getAttribBase( biTangentAttribName
           )->isLocked() );

                biTangents.resize( 1, Ra::Core::Vector3::Zero() );
                biTangents[0]  = Ra::Core::Vector3().setRandom();
                auto saveBiTan = biTangents[0];
                geometry->getAttribManager().getAttribBase( biTangentAttribName )->unlock();
                REQUIRE( !biTangents.empty() );


                        auto attribHandlerBiTan =
                            geometry->getAttribManager().findAttrib<Ra::Core::Vector3>(
           biTangentAttribName
                ); const auto& dataBiTan = geometry->getAttribManager().getAttrib(
           attribHandlerBiTan
                ).data(); REQUIRE( saveBiTan == dataBiTan[0] );

                        auto geoBiTanTest = new GeometryData();
                        geoBiTanTest->setBitangents( biTangents );
                        auto attribHandlerBiTan2 =
                            geoBiTanTest->getAttribManager().findAttrib<Ra::Core::Vector3>(
                biTangentAttribName ); const auto& attribBiTan =
           geoBiTanTest->getAttribManager().getAttrib( attribHandlerBiTan2 );

                        REQUIRE( attribBiTan.data().size() == 1 );
                        REQUIRE( attribBiTan.data()[0] == saveBiTan );

                        // TexCoords test
                        auto& texCoords = geometry->getTexCoords();
                        REQUIRE( texCoords.empty() );
                        REQUIRE(
                            geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_TEXCOORD
                )->isLocked() );

                        texCoords.resize( 1, Ra::Core::Vector3::Zero() );
                        texCoords[0]       = Ra::Core::Vector3().setRandom();
                        auto saveTexCoords = texCoords[0];
                        geometry->getAttribManager().getAttribBase( MeshAttrib::VERTEX_TEXCOORD
           )->unlock(); REQUIRE( !texCoords.empty() );

                        auto& nameTexCoords = Ra::Core::Geometry::getAttribName(
           MeshAttrib::VERTEX_TEXCOORD
                ); auto attribHandlerTexCoords =
           geometry->getAttribManager().findAttrib<Ra::Core::Vector3>( nameTexCoords ); const auto&
           dataTexCoords = geometry->getAttribManager().getAttrib( attribHandlerTexCoords ).data();
           REQUIRE( saveTexCoords == dataTexCoords[0] );

                        auto geoTexCoordsTest = new GeometryData();
                        geoTexCoordsTest->setTextureCoordinates( texCoords );
                        auto attribHandlerTexCoords2 =
                            geoTexCoordsTest->getAttribManager().findAttrib<Ra::Core::Vector3>(
                nameTexCoords ); const auto& attribTexCoords =
                            geoTexCoordsTest->getAttribManager().getAttrib( attribHandlerTexCoords2
           ); REQUIRE( attribTexCoords.data().size() == 1 ); REQUIRE( attribTexCoords.data()[0] ==
           saveTexCoords );

                        geometry->getMultiIndexedGeometry()
                            .getAttrib<Ra::Core::Vector3>( tangentAttribName )
                            .getDataWithLock();
                        REQUIRE( geometry->getAttribManager().getAttribBase( tangentAttribName
           )->isLocked()
                );
                        {
                            auto unlocker = geometry->getAttribManager().getUnlocker();
                            geometry->getMultiIndexedGeometry()
                                .getAttrib<Ra::Core::Vector3>( getAttribName(
           MeshAttrib::VERTEX_BITANGENT ) ) .getDataWithLock(); REQUIRE(
           geometry->getAttribManager() .getAttribBase( getAttribName( MeshAttrib::VERTEX_BITANGENT
           ) )
                                         ->isLocked() );
                        }
                        REQUIRE( geometry->getAttribManager().getAttribBase( tangentAttribName
           )->isLocked()
                ); REQUIRE( !geometry->getAttribManager().getAttribBase( biTangentAttribName
           )->isLocked()
                );
                */
    }

    SECTION( "Edges test " ) {
        auto geometry = GeometryData();
        auto& edges   = geometry.getEdges();
        REQUIRE( edges.empty() );

        edges.resize( 1 );
        edges[0]  = Ra::Core::Vector2ui().setRandom();
        auto save = edges[0];
        geometry.indexedDataUnlock( GeometryData::GeometryType::LINE_MESH, "in_edge" );
        REQUIRE( !edges.empty() );

        const auto& data = geometry.getIndexedData<Ra::Core::Vector2ui>( "in_edge" );
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
        REQUIRE( faces.empty() );

        faces.resize( 1 );
        faces[0]  = Ra::Core::VectorNui().setRandom();
        auto save = faces[0];
        geometry.indexedDataUnlock( GeometryData::GeometryType::POLY_MESH, "in_face" );
        REQUIRE( !faces.empty() );

        const auto& data = geometry.getIndexedData<Ra::Core::VectorNui>( "in_face" );
        REQUIRE( save == data[0] );

        auto geometry2 = GeometryData();
        geometry2.setFaces( faces );
        //        geometry2.setIndexedData( GeometryData::GeometryType::POLY_MESH, faces, "in_face"
        //        );
        REQUIRE( geometry2.getFaces().size() == 1 );
        REQUIRE( geometry.getFaces()[0] == save );
    }

    SECTION( "Polyhedron test " ) {
        auto geometry     = GeometryData();
        auto& polyhedrons = geometry.getPolyhedra();
        REQUIRE( polyhedrons.empty() );

        polyhedrons.resize( 1 );
        polyhedrons[0] = Ra::Core::VectorNui().setRandom();
        auto save      = polyhedrons[0];
        geometry.indexedDataUnlock( GeometryData::GeometryType::POLY_MESH, "in_polyhedron" );
        REQUIRE( !polyhedrons.empty() );

        const auto& data = geometry.getIndexedData<Ra::Core::VectorNui>( "in_polyhedron" );
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
