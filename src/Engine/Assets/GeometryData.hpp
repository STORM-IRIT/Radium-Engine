#ifndef RADIUMENGINE_GEOMETRY_DATA_HPP
#define RADIUMENGINE_GEOMETRY_DATA_HPP

#include <string>
#include <vector>
#include <Core/Log/Log.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Debug/Loading/AssimpGeometryDataLoader.hpp>

namespace Ra {
namespace Asset {

struct MaterialData {
    MaterialData() :
    m_diffuse(),
    m_specular(),
    m_shininess(),
    m_texDiffuse(""),
    m_texSpecular(""),
    m_texShininess(""),
    m_texNormal(""),
    m_texOpacity(""),
    m_hasDiffuse( false ),
    m_hasSpecular( false ),
    m_hasShininess( false ) { }

    MaterialData( const MaterialData& material ) = default;

    /// QUERY
    inline bool hasDiffuse() const {
        return m_hasDiffuse;
    }

    inline bool hasSpecular() const {
        return m_hasSpecular;
    }

    inline bool hasShininess() const {
        return m_hasShininess;
    }

    inline bool hasDiffuseTexture() const {
        return ( m_texDiffuse != "" );
    }

    inline bool hasSpecularTexture() const {
        return ( m_texDiffuse != "" );
    }

    inline bool hasShininessTexture() const {
        return ( m_texDiffuse != "" );
    }

    inline bool hasNormalTexture() const {
        return ( m_texDiffuse != "" );
    }

    inline bool hasOpacityTexture() const {
        return ( m_texDiffuse != "" );
    }

    /// VARIABLE
    Core::Color m_diffuse;
    Core::Color m_specular;
    Scalar      m_shininess;
    std::string m_texDiffuse;
    std::string m_texSpecular;
    std::string m_texShininess;
    std::string m_texNormal;
    std::string m_texOpacity;
    bool        m_hasDiffuse;
    bool        m_hasSpecular;
    bool        m_hasShininess;
};



class GeometryData {
public:
    /// FRIEND
    friend class AssimpGeometryDataLoader;

    /// ENUM
    enum GeometryType {
        UNKNOWN     = 1 << 0,
        POINT_CLOUD = 1 << 1,
        LINE_MESH   = 1 << 2,
        TRI_MESH    = 1 << 3,
        QUAD_MESH   = 1 << 4,
        POLY_MESH   = 1 << 5,
        TETRA_MESH  = 1 << 6,
        HEX_MESH    = 1 << 7
    };

    /// CONSTRUCTOR
    GeometryData( const std::string&  name = "",
                  const GeometryType& type = UNKNOWN ) :
        m_name( name ),
        m_type( type ),
        m_frame( Core::Transform::Identity() ),
        m_vertex(),
        m_edge(),
        m_face(),
        m_polyhedron(),
        m_normal(),
        m_tangent(),
        m_bitangent(),
        m_texCoord(),
        m_color(),
        m_material(),
        m_hasMaterial( false ) { }

    /// NAME
    inline std::string getName() const {
        return m_name;
    }

    /// TYPE
    inline GeometryType getType() const {
        return m_type;
    }

    /// FRAME
    inline Core::Transform getFrame() const {
        return m_frame;
    }

    /// DATA
    inline uint getVerticesSize() const {
        return m_vertex.size();
    }

    inline std::vector< Core::Vector3 > getVertices() const {
        return m_vertex;
    }

    inline std::vector< Core::Vector2i > getEdges() const {
        return m_edge;
    }

    inline std::vector< Core::VectorNi > getFaces() const {
        return m_face;
    }

    inline std::vector< Core::VectorNi > getPolyhedra() const {
        return m_polyhedron;
    }

    inline std::vector< Core::Vector3 > getNormals() const {
        return m_normal;
    }

    inline std::vector< Core::Vector4 > getTangents() const {
        return m_tangent;
    }

    inline std::vector< Core::Vector4 > getBiTangents() const {
        return m_bitangent;
    }

    inline std::vector< Core::Vector4 > getTextureCoordinates() const {
        return m_texCoord;
    }

    inline std::vector< Core::Color > getColors() const {
        return m_color;
    }

    inline MaterialData getMaterial() const {
        return m_material;
    }

    /// QUERY
    inline bool isPointCloud() const {
        return ( type == POINT_CLOUD );
    }

    inline bool isLineMesh() const {
        return ( type == LINE_MESH );
    }

    inline bool isTriMesh() const {
        return ( type == TRI_MESH );
    }

    inline bool isQuadMesh() const {
        return ( type == QUAD_MESH );
    }

    inline bool isPolyMesh() const {
        return ( type == POLY_MESH );
    }

    inline bool isTetraMesh() const {
        return ( type == TETRA_MESH );
    }

    inline bool isHexMesh() const {
        return ( type == HEX_MESH );
    }

    inline bool hasVertices() const {
        return !m_vertex.empty();
    }

    inline bool hasEdges() const {
        return !m_edge.empty();
    }

    inline bool hasFaces() const {
        return !m_face.empty();
    }

    inline bool hasPolyhedra() const {
        return !m_polyhedron.empty();
    }

    inline bool hasNormals() const {
        return !m_normal.empty();
    }

    inline bool hasTangents() const {
        return !m_tangent.empty();
    }

    inline bool hasBiTangents() const {
        return !m_bitangent.empty();
    }

    inline bool hasTextureCoordinates() const {
        return !m_texCoord.empty();
    }

    inline bool hasColors() const {
        return !m_color.empty();
    }

    inline bool hasMaterial() const {
        return m_hasMaterial;
    }

    /// DEBUG
    inline void displayInfo() const {
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
        LOG( logDEBUG ) << " Face #       : " << m_face.size();
        LOG( logDEBUG ) << " Normal ?     : " << ( m_normal.empty()    ) ? "NO" : "YES";
        LOG( logDEBUG ) << " Tangent ?    : " << ( m_tangent.empty()   ) ? "NO" : "YES";
        LOG( logDEBUG ) << " Bitangent ?  : " << ( m_bitangent.empty() ) ? "NO" : "YES";
        LOG( logDEBUG ) << " Tex.Coord. ? : " << ( m_texCoord.empty()  ) ? "NO" : "YES";
        LOG( logDEBUG ) << " Color ?      : " << ( m_color.empty()     ) ? "NO" : "YES";
        LOG( logDEBUG ) << " Material ?   : " << ( !m_hasMaterial      ) ? "NO" : "YES";
    }

protected:
    /// NAME
    inline void setName( const std::string& name ) {
        m_name = name;
    }

    /// TYPE
    inline void setType( const GeometryType& type ) {
        m_type = type;
    }

    /// FRAME
    inline void setFrame( const Core::Transform& frame ) {
        m_frame = frame;
    }

    /// VERTEX
    inline void setVertices( const std::vector< Core::Vector3 >& vertexList ) {
        const uint size = vertexList.size();
        m_vertex.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_vertex[i] = vertexList[i];
        }
    }

    /// EDGE
    inline void setEdges( const std::vector< Core::Vector2i >& edgeList ) {
        const uint size = edgeList.size();
        m_edge.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_edge[i] = edgeList[i];
        }
    }

    /// FACE
    inline void setFaces( const std::vector< Core::VectorNi >& faceList ) {
        const uint size = faceList.size();
        m_face.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_face[i] = faceList[i];
        }
    }

    /// POLYHEDRON
    inline void setPolyhedron( const std::vector< Core::VectorNi >& polyList ) {
        const uint size = polyList.size();
        m_polyhedron.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_polyhedron[i] = polyList[i];
        }
    }

    /// NORMAL
    inline void setNormals( const std::vector< Core::Vector3 >& normalList ) {
        const uint size = normalList.size();
        m_normal.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_normal[i] = normalList[i];
        }
    }

    /// TANGENT
    inline void setTangents( const std::vector< Core::Vector4 >& tangentList ) {
        const uint size = tangentList.size();
        m_tangent.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_tangent[i] = tangentList[i];
        }
    }

    /// BITANGENT
    inline void setBitangents( const std::vector< Core::Vector4 >& bitangentList ) {
        const uint size = bitangentList.size();
        m_bitangent.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_bitangent[i] = bitangentList[i];
        }
    }

    /// TEXTURE COORDINATE
    inline void setTextureCoordinates( const std::vector< Core::Vector4 >& texCoordList ) {
        const uint size = texCoordList.size();
        m_texCoord.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_texCoord[i] = texCoordList[i];
        }
    }

    /// COLOR
    inline void setColors( const std::vector< Core::Color >& colorList ) {
        const uint size = colorList.size();
        m_color.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_color[i] = colorList[i];
        }
    }

    /// MATERIAL
    inline void setMaterial( const MaterialData& material ) {
        m_material = material;
    }

protected:
    /// VARIABLE
    std::string     m_name;
    GeometryType    m_type;
    Core::Transform m_frame;

    std::vector< Core::Vector3  > m_vertex;
    std::vector< Core::Vector2i > m_edge;
    std::vector< Core::VectorNi > m_face;
    std::vector< Core::VectorNi > m_polyhedron;
    std::vector< Core::Vector3  > m_normal;
    std::vector< Core::Vector4  > m_tangent;
    std::vector< Core::Vector4  > m_bitangent;
    std::vector< Core::Vector4  > m_texCoord;
    std::vector< Core::Color    > m_color;

    MaterialData                  m_material;
    bool                          m_hasMaterial;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
