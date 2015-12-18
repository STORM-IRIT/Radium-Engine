#include <Engine/Assets/GeometryData.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Asset {



////////////////
/// MATERIAL ///
////////////////

/// QUERY
inline bool MaterialData::hasDiffuse() const {
    return m_hasDiffuse;
}

inline bool MaterialData::hasSpecular() const {
    return m_hasSpecular;
}

inline bool MaterialData::hasShininess() const {
    return m_hasShininess;
}

inline bool MaterialData::hasDiffuseTexture() const {
    return ( m_texDiffuse != "" );
}

inline bool MaterialData::hasSpecularTexture() const {
    return ( m_texSpecular != "" );
}

inline bool MaterialData::hasShininessTexture() const {
    return ( m_texShininess != "" );
}

inline bool MaterialData::hasNormalTexture() const {
    return ( m_texNormal != "" );
}

inline bool MaterialData::hasOpacityTexture() const {
    return ( m_texOpacity != "" );
}



/////////////////////
/// GEOMETRY DATA ///
/////////////////////

/// TYPE
inline GeometryData::GeometryType GeometryData::getType() const {
    return m_type;
}

/// FRAME
inline Core::Transform GeometryData::getFrame() const {
    return m_frame;
}

/// DATA
inline uint GeometryData::getVerticesSize() const {
    return m_vertex.size();
}

inline const GeometryData::Vector3Array& GeometryData::getVertices() const
{
    return m_vertex;
}

inline const GeometryData::Vector2iArray& GeometryData::getEdges() const
{
    return m_edge;
}

inline const GeometryData::VectorNiArray& GeometryData::getFaces() const
{
    return m_faces;
}

inline const GeometryData::VectorNiArray& GeometryData::getPolyhedra() const
{
    return m_polyhedron;
}

inline const GeometryData::Vector3Array& GeometryData::getNormals() const
{
    return m_normal;
}

inline const GeometryData::Vector4Array& GeometryData::getTangents() const
{
    return m_tangent;
}

inline const GeometryData::Vector4Array& GeometryData::getBiTangents() const
{
    return m_bitangent;
}

inline const GeometryData::Vector4Array& GeometryData::getTexCoords() const
{
    return m_texCoord;
}

inline const GeometryData::ColorArray& GeometryData::getColors() const
{
    return m_color;
}

inline const MaterialData& GeometryData::getMaterial() const
{
    return m_material;
}

/// QUERY
inline bool GeometryData::isPointCloud() const {
    return ( m_type == POINT_CLOUD );
}

inline bool GeometryData::isLineMesh() const {
    return ( m_type == LINE_MESH );
}

inline bool GeometryData::isTriMesh() const {
    return ( m_type == TRI_MESH );
}

inline bool GeometryData::isQuadMesh() const {
    return ( m_type == QUAD_MESH );
}

inline bool GeometryData::isPolyMesh() const {
    return ( m_type == POLY_MESH );
}

inline bool GeometryData::isTetraMesh() const {
    return ( m_type == TETRA_MESH );
}

inline bool GeometryData::isHexMesh() const {
    return ( m_type == HEX_MESH );
}

inline bool GeometryData::hasVertices() const {
    return !m_vertex.empty();
}

inline bool GeometryData::hasEdges() const {
    return !m_edge.empty();
}

inline bool GeometryData::hasFaces() const {
    return !m_faces.empty();
}

inline bool GeometryData::hasPolyhedra() const {
    return !m_polyhedron.empty();
}

inline bool GeometryData::hasNormals() const {
    return !m_normal.empty();
}

inline bool GeometryData::hasTangents() const {
    return !m_tangent.empty();
}

inline bool GeometryData::hasBiTangents() const {
    return !m_bitangent.empty();
}

inline bool GeometryData::hasTextureCoordinates() const {
    return !m_texCoord.empty();
}

inline bool GeometryData::hasColors() const {
    return !m_color.empty();
}

inline bool GeometryData::hasMaterial() const {
    return m_hasMaterial;
}

/// DEBUG
inline void GeometryData::displayInfo() const {
    std::string type;
    switch( m_type ) {
        case UNKNOWN     : type = "UNKNOWN";       break;
        case POINT_CLOUD : type = "POINT CLOUD";   break;
        case LINE_MESH   : type = "LINE MESH";     break;
        case TRI_MESH    : type = "TRIANGLE MESH"; break;
        case QUAD_MESH   : type = "QUAD MESH";     break;
        case POLY_MESH   : type = "POLY MESH";     break;
        case TETRA_MESH  : type = "TETRA MESH";    break;
        case HEX_MESH    : type = "HEX MESH";      break;
    }
    LOG( logDEBUG ) << "======== MESH INFO ========";
    LOG( logDEBUG ) << " Name         : " << m_name;
    LOG( logDEBUG ) << " Type         : " << type;
    LOG( logDEBUG ) << " Vertex #     : " << m_vertex.size();
    LOG( logDEBUG ) << " Edge #       : " << m_edge.size();
    LOG( logDEBUG ) << " Face #       : " << m_faces.size();
    LOG( logDEBUG ) << " Normal ?     : " << ( ( m_normal.empty()    ) ? "NO" : "YES" );
    LOG( logDEBUG ) << " Tangent ?    : " << ( ( m_tangent.empty()   ) ? "NO" : "YES" );
    LOG( logDEBUG ) << " Bitangent ?  : " << ( ( m_bitangent.empty() ) ? "NO" : "YES" );
    LOG( logDEBUG ) << " Tex.Coord. ? : " << ( ( m_texCoord.empty()  ) ? "NO" : "YES" );
    LOG( logDEBUG ) << " Color ?      : " << ( ( m_color.empty()     ) ? "NO" : "YES" );
    LOG( logDEBUG ) << " Material ?   : " << ( ( !m_hasMaterial      ) ? "NO" : "YES" );

    if( m_hasMaterial ) {
        std::string kd;
        std::string ks;
        std::string ns;
        if( m_material.hasDiffuse() ) {
            Core::StringUtils::stringPrintf( kd, "%.3f %.3f %.3f %.3f",
                                             m_material.m_diffuse.x(),
                                             m_material.m_diffuse.y(),
                                             m_material.m_diffuse.z(),
                                             m_material.m_diffuse.w() );
        }
        if( m_material.hasSpecular() ) {
            Core::StringUtils::stringPrintf( ks, "%.3f %.3f %.3f %.3f",
                                             m_material.m_specular.x(),
                                             m_material.m_specular.y(),
                                             m_material.m_specular.z(),
                                             m_material.m_specular.w() );
        }
        if( m_material.hasShininess() ) {
            Core::StringUtils::stringPrintf( ns, "%.1f", m_material.m_shininess );
        }
        LOG( logDEBUG ) << "======== MATERIAL INFO ========";
        LOG( logDEBUG ) << " Kd             : " << ( ( m_material.hasDiffuse() )          ? kd                        : "NO" );
        LOG( logDEBUG ) << " Ks             : " << ( ( m_material.hasSpecular() )         ? ks                        : "NO" );
        LOG( logDEBUG ) << " Ns             : " << ( ( m_material.hasShininess() )        ? ns                        : "NO" );
        LOG( logDEBUG ) << " Kd Texture     : " << ( ( m_material.hasDiffuseTexture() )   ? m_material.m_texDiffuse   : "NO" );
        LOG( logDEBUG ) << " Ks Texture     : " << ( ( m_material.hasSpecularTexture() )  ? m_material.m_texSpecular  : "NO" );
        LOG( logDEBUG ) << " Ns Texture     : " << ( ( m_material.hasShininessTexture() ) ? m_material.m_texShininess : "NO" );
        LOG( logDEBUG ) << " Normal Texture : " << ( ( m_material.hasNormalTexture() )    ? m_material.m_texNormal    : "NO" );
        LOG( logDEBUG ) << " Alpha Texture  : " << ( ( m_material.hasOpacityTexture() )   ? m_material.m_texOpacity   : "NO" );
    }
}

/// NAME
inline void GeometryData::setName( const std::string& name ) {
    m_name = name;
}

/// TYPE
inline void GeometryData::setType( const GeometryType& type ) {
    m_type = type;
}

/// FRAME
inline void GeometryData::setFrame( const Core::Transform& frame ) {
    m_frame = frame;
}

/// VERTEX
inline void GeometryData::setVertices( const std::vector< Core::Vector3 >& vertexList ) {
    const uint size = vertexList.size();
    m_vertex.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_vertex[i] = vertexList[i];
    }
}

/// EDGE
inline void GeometryData::setEdges( const std::vector< Core::Vector2i >& edgeList ) {
    const uint size = edgeList.size();
    m_edge.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_edge[i] = edgeList[i];
    }
}

/// FACE
inline void GeometryData::setFaces( const std::vector< Core::VectorNi >& faceList ) {
    const uint size = faceList.size();
    m_faces.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_faces[i] = faceList[i];
    }
}

/// POLYHEDRON
inline void GeometryData::setPolyhedron( const std::vector< Core::VectorNi >& polyList ) {
    const uint size = polyList.size();
    m_polyhedron.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_polyhedron[i] = polyList[i];
    }
}

/// NORMAL
inline void GeometryData::setNormals( const std::vector< Core::Vector3 >& normalList ) {
    const uint size = normalList.size();
    m_normal.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_normal[i] = normalList[i];
    }
}

/// TANGENT
inline void GeometryData::setTangents( const std::vector< Core::Vector4 >& tangentList ) {
    const uint size = tangentList.size();
    m_tangent.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_tangent[i] = tangentList[i];
    }
}

/// BITANGENT
inline void GeometryData::setBitangents( const std::vector< Core::Vector4 >& bitangentList ) {
    const uint size = bitangentList.size();
    m_bitangent.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_bitangent[i] = bitangentList[i];
    }
}

/// TEXTURE COORDINATE
inline void GeometryData::setTextureCoordinates( const std::vector< Core::Vector4 >& texCoordList ) {
    const uint size = texCoordList.size();
    m_texCoord.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_texCoord[i] = texCoordList[i];
    }
}

/// COLOR
inline void GeometryData::setColors( const std::vector< Core::Color >& colorList ) {
    const uint size = colorList.size();
    m_color.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_color[i] = colorList[i];
    }
}

/// MATERIAL
inline void GeometryData::setMaterial( const MaterialData& material ) {
    m_material = material;
    m_hasMaterial = true;
}


} // namespace Asset
} // namespace Ra
