#ifndef RADIUMENGINE_GEOMETRY_DATA_HPP
#define RADIUMENGINE_GEOMETRY_DATA_HPP

#include <string>
#include <vector>

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>

#include <Core/File/AssetData.hpp>

namespace Ra {
namespace Asset {

struct RA_CORE_API MaterialData {
    MaterialData();
    MaterialData( const MaterialData& material ) = default;

    /// QUERY
    inline bool hasDiffuse() const;
    inline bool hasSpecular() const;
    inline bool hasShininess() const;
    inline bool hasOpacity() const;
    inline bool hasDiffuseTexture() const;
    inline bool hasSpecularTexture() const;
    inline bool hasShininessTexture() const;
    inline bool hasNormalTexture() const;
    inline bool hasOpacityTexture() const;

    /// VARIABLE
    Core::Color m_diffuse;
    Core::Color m_specular;
    Scalar      m_shininess;
    Scalar      m_opacity;
    std::string m_texDiffuse;
    std::string m_texSpecular;
    std::string m_texShininess;
    std::string m_texNormal;
    std::string m_texOpacity;
    bool        m_hasDiffuse;
    bool        m_hasSpecular;
    bool        m_hasShininess;
    bool        m_hasOpacity;
    bool        m_hasTexDiffuse;
    bool        m_hasTexSpecular;
    bool        m_hasTexShininess;
    bool        m_hasTexNormal;
    bool        m_hasTexOpacity;
};

class RA_CORE_API GeometryData : public AssetData {

public:
    using Vector3Array  = Core::VectorArray<Core::Vector3>  ;
    using Vector2iArray = Core::VectorArray<Core::Vector2i> ;
    using Vector2uArray = Core::VectorArray<Core::Vector2ui>;
    using VectorNiArray = Core::VectorArray<Core::VectorNi> ;
    using VectorNuArray = Core::VectorArray<Core::VectorNui>;
    using Vector4Array  = Core::VectorArray<Core::Vector4>  ;
    using ColorArray    = Core::VectorArray<Core::Color>    ;

    using Weight        = std::pair<Scalar, uint>;
    using VertexWeights = std::vector<Weight>;
    using WeightArray   = std::vector<VertexWeights>;

public:

    RA_CORE_ALIGNED_NEW

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

    /// NAME
    inline void setName( const std::string& name );

    /// TYPE
    inline GeometryType getType() const;
    inline void setType( const GeometryType& type );

    /// FRAME
    inline Core::Transform getFrame() const;
    inline void setFrame( const Core::Transform& frame );

    /// DATA
    inline uint getVerticesSize() const;

    inline const Vector3Array& getVertices() const;
    inline void setVertices( const std::vector< Core::Vector3 >& vertexList );

    inline const Vector2uArray& getEdges() const;
    inline void setEdges( const std::vector< Core::Vector2ui >& edgeList );

    inline const VectorNuArray& getFaces() const;
    inline void setFaces( const std::vector< Core::VectorNui >& faceList );

    inline const VectorNuArray& getPolyhedra() const;
    inline void setPolyhedron( const std::vector< Core::VectorNui >& polyList );

    inline const Vector3Array& getNormals() const;
    inline void setNormals( const std::vector< Core::Vector3 >& normalList );

    inline const Vector3Array& getTangents() const;
    inline void setTangents( const std::vector< Core::Vector3 >& tangentList );

    inline const Vector3Array& getBiTangents() const;
    inline void setBitangents( const std::vector< Core::Vector3 >& bitangentList );

    inline const Vector3Array& getTexCoords() const;
    inline void setTextureCoordinates( const std::vector< Core::Vector3 >& texCoordList );

    inline const ColorArray& getColors() const;
    inline void setColors( const std::vector< Core::Color >& colorList );

    inline const WeightArray& getWeights() const;
    inline void setWeights(const WeightArray& weightList);

    inline const MaterialData& getMaterial() const;
    inline void setMaterial( const MaterialData& material );


    /// DUPLICATES
    inline const std::map< uint, uint >& getDuplicateTable() const;
    inline void setDuplicateTable( const std::map< uint, uint >& table );
    inline void setLoadDuplicates( const bool status );

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
    inline bool hasWeights() const;
    inline bool hasMaterial() const;
    inline bool isLoadingDuplicates() const;

    /// DEBUG
    inline void displayInfo() const;

protected:
    /// VARIABLE
    Core::Transform m_frame;
    GeometryType    m_type;

    Vector3Array  m_vertex;
    Vector2uArray m_edge;
    VectorNuArray m_faces;
    VectorNuArray m_polyhedron;
    Vector3Array  m_normal;
    Vector3Array  m_tangent;
    Vector3Array  m_bitangent;
    Vector3Array  m_texCoord;
    ColorArray    m_color;
    WeightArray   m_weights;

    MaterialData m_material;
    bool         m_hasMaterial;

    std::map< uint, uint > m_duplicateTable;
    bool                   m_loadDuplicates;
};

} // namespace Asset
} // namespace Ra

#include <Core/File/GeometryData.inl>

#endif // RADIUMENGINE_GEOMETRY_DATA_HPP
