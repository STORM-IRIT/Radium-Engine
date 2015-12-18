#ifndef RADIUMENGINE_GEOMETRY_DATA_HPP
#define RADIUMENGINE_GEOMETRY_DATA_HPP

#include <string>
#include <vector>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>

#include <Engine/Assets/AssimpGeometryDataLoader.hpp>
#include <Engine/Assets/AssetData.hpp>

namespace Ra {
namespace Asset {

struct MaterialData {
    MaterialData();
    MaterialData( const MaterialData& material ) = default;

    /// QUERY
    inline bool hasDiffuse() const;
    inline bool hasSpecular() const;
    inline bool hasShininess() const;
    inline bool hasDiffuseTexture() const;
    inline bool hasSpecularTexture() const;
    inline bool hasShininessTexture() const;
    inline bool hasNormalTexture() const;
    inline bool hasOpacityTexture() const;

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

class GeometryData : public AssetData {

    /// FRIEND
    friend class AssimpGeometryDataLoader;

    using Vector3Array  = Core::VectorArray<Core::Vector3> ;
    using Vector2iArray = Core::VectorArray<Core::Vector2i>;
    using VectorNiArray = Core::VectorArray<Core::VectorNi>;
    using Vector4Array  = Core::VectorArray<Core::Vector4> ;
    using ColorArray    = Core::VectorArray<Core::Color>   ;

public:

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
                  const GeometryType& type = UNKNOWN );

    GeometryData( const GeometryData& data ) = default;

    /// DESTRUCTOR
    ~GeometryData();

    /// TYPE
    inline GeometryType getType() const;

    /// FRAME
    inline Core::Transform getFrame() const;

    /// DATA
    inline uint getVerticesSize()               const;
    inline const Vector3Array & getVertices()   const;
    inline const Vector2iArray& getEdges()      const;
    inline const VectorNiArray& getFaces()      const;
    inline const VectorNiArray& getPolyhedra()  const;
    inline const Vector3Array & getNormals()    const;
    inline const Vector4Array & getTangents()   const;
    inline const Vector4Array & getBiTangents() const;
    inline const Vector4Array & getTexCoords()  const;
    inline const ColorArray   & getColors()     const;
    inline const MaterialData & getMaterial()   const;

    /// QUERY
    inline bool isPointCloud() const;
    inline bool isLineMesh() const;
    inline bool isTriMesh() const;
    inline bool isQuadMesh() const;
    inline bool isPolyMesh() const;
    inline bool isTetraMesh() const;
    inline bool isHexMesh() const;
    inline bool hasVertices() const;
    inline bool hasEdges() const;
    inline bool hasFaces() const;
    inline bool hasPolyhedra() const;
    inline bool hasNormals() const;
    inline bool hasTangents() const;
    inline bool hasBiTangents() const;
    inline bool hasTextureCoordinates() const;
    inline bool hasColors() const;
    inline bool hasMaterial() const;

    /// DEBUG
    inline void displayInfo() const;

protected:
    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline void setType( const GeometryType& type );

    /// FRAME
    inline void setFrame( const Core::Transform& frame );

    /// VERTEX
    inline void setVertices( const std::vector< Core::Vector3 >& vertexList );

    /// EDGE
    inline void setEdges( const std::vector< Core::Vector2i >& edgeList );

    /// FACE
    inline void setFaces( const std::vector< Core::VectorNi >& faceList );

    /// POLYHEDRON
    inline void setPolyhedron( const std::vector< Core::VectorNi >& polyList );

    /// NORMAL
    inline void setNormals( const std::vector< Core::Vector3 >& normalList );

    /// TANGENT
    inline void setTangents( const std::vector< Core::Vector4 >& tangentList );

    /// BITANGENT
    inline void setBitangents( const std::vector< Core::Vector4 >& bitangentList );

    /// TEXTURE COORDINATE
    inline void setTextureCoordinates( const std::vector< Core::Vector4 >& texCoordList );

    /// COLOR
    inline void setColors( const std::vector< Core::Color >& colorList );

    /// MATERIAL
    inline void setMaterial( const MaterialData& material );

protected:
    /// VARIABLE
    //std::string     m_name;
    GeometryType    m_type;
    Core::Transform m_frame;

    Vector3Array  m_vertex;
    Vector2iArray m_edge;
    VectorNiArray m_faces;
    VectorNiArray m_polyhedron;
    Vector3Array  m_normal;
    Vector4Array  m_tangent;
    Vector4Array  m_bitangent;
    Vector4Array  m_texCoord;
    ColorArray    m_color;

    MaterialData                  m_material;
    bool                          m_hasMaterial;
};

} // namespace Asset
} // namespace Ra

#include <Engine/Assets/GeometryData.inl>

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
