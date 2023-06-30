#include <Core/Asset/GeometryData.hpp>

#include <Core/Geometry/StandardAttribNames.hpp>
#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

GeometryData::GeometryData( const std::string& name, const GeometryType& type ) :
    AssetData( name ),
    m_frame( Transform::Identity() ),
    m_type( type ),
    m_geometry(),
    m_material() {}

void GeometryData::displayInfo() const {
    using namespace Core::Utils; // log
    std::string type;
    switch ( m_type ) {
    case POINT_CLOUD:
        type = "POINT CLOUD";
        break;
    case LINE_MESH:
        type = "LINE MESH";
        break;
    case TRI_MESH:
        type = "TRIANGLE MESH";
        break;
    case QUAD_MESH:
        type = "QUAD MESH";
        break;
    case POLY_MESH:
        type = "POLY MESH";
        break;
    case TETRA_MESH:
        type = "TETRA MESH";
        break;
    case HEX_MESH:
        type = "HEX MESH";
        break;
    case UNKNOWN:
    default:
        type = "UNKNOWN";
        break;
    }

    auto attribSize = [this]( Geometry::MeshAttrib a ) -> size_t {
        const auto& name = getAttribName( a );
        return getGeometry().hasAttrib( name ) ? getGeometry().getAttribBase( name )->getSize() : 0;
    };

    auto hasAttrib = [this]( Geometry::MeshAttrib a ) -> std::string {
        return getGeometry().hasAttrib( getAttribName( a ) ) ? "YES" : "NO";
    };

    using namespace Geometry;
    LOG( logINFO ) << "======== MESH INFO ========";
    LOG( logINFO ) << " Name           : " << getName();
    LOG( logINFO ) << " Type           : " << type;
    LOG( logINFO ) << " Edge #         : " << ( hasEdges() ? getPrimitiveCount() : 0 );
    LOG( logINFO ) << " Face #         : " << ( hasFaces() ? getPrimitiveCount() : 0 );
    LOG( logINFO ) << " Vertex #       : " << attribSize( MeshAttrib::VERTEX_POSITION );
    LOG( logINFO ) << " Normal ?       : " << hasAttrib( MeshAttrib::VERTEX_NORMAL );
    LOG( logINFO ) << " Tangent ?      : " << hasAttrib( MeshAttrib::VERTEX_TANGENT );
    LOG( logINFO ) << " Bitangent ?    : " << hasAttrib( MeshAttrib::VERTEX_BITANGENT );
    LOG( logINFO ) << " Tex.Coord. ?   : " << hasAttrib( MeshAttrib::VERTEX_TEXCOORD );
    LOG( logINFO ) << " Color ?        : " << hasAttrib( MeshAttrib::VERTEX_COLOR );
    LOG( logINFO ) << " Material ?     : " << ( ( !hasMaterial() ) ? "NO" : "YES" );

    if ( hasMaterial() ) { m_material.displayInfo(); }
}
} // namespace Asset
} // namespace Core
} // namespace Ra
