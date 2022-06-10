#pragma once
#include "Core/Geometry/StandardAttribNames.hpp"
#include <Core/Asset/GeometryData.hpp>

#include <algorithm> //std::transform

namespace Ra {
namespace Core {
namespace Asset {

inline void GeometryData::setName( const std::string& name ) {
    m_name = name;
}

inline GeometryData::GeometryType GeometryData::getType() const {
    return m_type;
}

inline void GeometryData::setType( const GeometryType& type ) {
    m_type = type;
}

inline Transform GeometryData::getFrame() const {
    return m_frame;
}

inline void GeometryData::setFrame( const Transform& frame ) {
    m_frame = frame;
}

inline const MaterialData& GeometryData::getMaterial() const {
    return *( m_material.get() );
}

inline void GeometryData::setMaterial( MaterialData* material ) {
    m_material.reset( material );
}

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

inline bool GeometryData::hasEdges() const {
    return m_geometry.containsLayer( { Geometry::LineIndexLayer::staticSemanticName }, "indices" );
}

inline bool GeometryData::hasFaces() const {
    std::string layerSemanticName;
    switch ( m_type ) {
    case TRI_MESH:
        layerSemanticName = std::string( Geometry::TriangleIndexLayer::staticSemanticName );
        break;
    case QUAD_MESH:
        layerSemanticName = std::string( Geometry::QuadIndexLayer::staticSemanticName );
        break;
    case POLY_MESH:
        layerSemanticName = std::string( Geometry::PolyIndexLayer::staticSemanticName );
        break;
    default:
        return false;
    }
    return m_geometry.containsLayer( { layerSemanticName }, "indices" );
}

inline bool GeometryData::hasPolyhedra() const {
    return m_geometry.containsLayer( { Geometry::PolyIndexLayer::staticSemanticName }, "indices" );
}

inline bool GeometryData::hasMaterial() const {
    return m_material != nullptr;
}

const Geometry::MultiIndexedGeometry& GeometryData::getGeometry() const {
    return m_geometry;
}

Geometry::MultiIndexedGeometry& GeometryData::getGeometry() {
    return m_geometry;
}

inline void GeometryData::setPrimitiveCount( int n ) {
    m_primitiveCount = n;
}
inline int GeometryData::getPrimitiveCount() const {
    return m_primitiveCount;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
