#pragma once
#include <Core/RaCore.hpp>

#include <Core/Asset/AssetData.hpp>
#include <Core/Asset/MaterialData.hpp>
#include <Core/Geometry/IndexedGeometry.hpp>
#include <Core/Types.hpp>

#include <string>

namespace Ra {
namespace Core {
using namespace Geometry;

namespace Asset {
/**
 * The GeometryData class stores all the geometry related data of a loaded object.
 */
class RA_CORE_API GeometryData : public AssetData
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * The type of geometry.
     */
    enum GeometryType : unsigned int {
        UNKNOWN     = 1u << 0u,
        POINT_CLOUD = 1u << 1u,
        LINE_MESH   = 1u << 2u,
        TRI_MESH    = 1u << 3u,
        QUAD_MESH   = 1u << 4u,
        POLY_MESH   = 1u << 5u,
        TETRA_MESH  = 1u << 6u,
        HEX_MESH    = 1u << 7u
    };

    explicit GeometryData( const std::string& name = "", const GeometryType& type = UNKNOWN );

    GeometryData( const GeometryData& data ) = delete;

    /// \name Data access
    /// \{

    /// Return the type of geometry.
    inline GeometryType getType() const { return m_type; }

    /// Set the type of geometry.
    inline void setType( const GeometryType& type ) { m_type = type; }

    /// Return the Transform of the object.
    inline Transform getFrame() const { return m_frame; }

    /// Set the Transform of the object.
    inline void setFrame( const Transform& frame ) { m_frame = frame; }

    /// Return the MaterialData associated to the objet.
    inline const MaterialData& getMaterial() const { return m_material; }

    /// Set the MaterialData for the object.
    inline void setMaterial( MaterialData material ) { m_material = material; }

    /// Read/write access to the multiIndexedGeometry;
    inline MultiIndexedGeometry& getGeometry() { return m_geometry; }

    /// Read only access to the multiIndexedGeometry;
    inline const MultiIndexedGeometry& getGeometry() const { return m_geometry; }

    /// \}

    /// \name Status queries
    /// \{

    /// Return true if the object is a Point Cloud.
    inline bool isPointCloud() const { return m_type == POINT_CLOUD; }

    /// Return true if the object is a Line Mesh.
    inline bool isLineMesh() const { return m_type == LINE_MESH; }

    /// Return true if the object is a Triangle Mesh.
    inline bool isTriMesh() const { return m_type == TRI_MESH; }

    /// Return true if the object is a Quadrangle Mesh.
    inline bool isQuadMesh() const { return m_type == QUAD_MESH; }

    /// Return true if the object is a Polygon Mesh.
    /// \note Return false for Triangle and Quadrangle meshes.
    inline bool isPolyMesh() const { return m_type == POLY_MESH; }

    /// Return true if the object is a Tetrahedron Mesh.
    inline bool isTetraMesh() const { return m_type == TETRA_MESH; }

    /// Return true if the object is a hexahedron Mesh.
    inline bool isHexMesh() const { return m_type == HEX_MESH; }

    /// Return true if the object has lines.
    inline bool hasEdges() const;

    /// Return true if the object has faces.
    inline bool hasFaces() const;

    /// Return true if the object has polyhedra.
    inline bool hasPolyhedra() const;

    /// Return true if the object has MaterialData.
    inline bool hasMaterial() const { return m_material.getMaterialModel() != nullptr; }

    /// Used to track easily the number of primitives in the geometry data
    inline void setPrimitiveCount( int n ) { m_primitiveCount = n; }

    /// Return the number of primitives in the geometry data
    inline int getPrimitiveCount() const { return m_primitiveCount; }

    /// \}

    /// Print stat info to the Debug output.
    void displayInfo() const;

  protected:
    /// The transformation of the object.
    Transform m_frame;

    /// The type of geometry for the object.
    GeometryType m_type;

    /// Named attributes
    MultiIndexedGeometry m_geometry;

    /// Simple tracking of geometric primitive number
    int m_primitiveCount { -1 };

    /// The MaterialData for the object.
    MaterialData m_material;
};

inline bool GeometryData::hasEdges() const {
    return m_geometry.containsLayer( { LineIndexLayer::staticSemanticName }, "indices" );
}

inline bool GeometryData::hasFaces() const {
    std::string layerSemanticName;
    switch ( m_type ) {
    case TRI_MESH:
        layerSemanticName = std::string( TriangleIndexLayer::staticSemanticName );
        break;
    case QUAD_MESH:
        layerSemanticName = std::string( QuadIndexLayer::staticSemanticName );
        break;
    case POLY_MESH:
        layerSemanticName = std::string( PolyIndexLayer::staticSemanticName );
        break;
    default:
        return false;
    }
    return m_geometry.containsLayer( { layerSemanticName }, "indices" );
}

inline bool GeometryData::hasPolyhedra() const {
    return m_geometry.containsLayer( { PolyIndexLayer::staticSemanticName }, "indices" );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
