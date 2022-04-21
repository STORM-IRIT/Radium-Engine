#pragma once
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

inline std::size_t GeometryData::getVerticesSize() const {
    return getAttribData<const Vector3Array&>( "vertex" ).size();
}

inline const Vector3Array& GeometryData::getVertices() const {
    return getAttribData<const Vector3Array&>( "vertex" );
}

inline Vector3Array& GeometryData::getVertices() {
    return getAttribDataWithLock<Vector3Array&>( "vertex" );
}

namespace internal {

template <typename InContainer, typename OutContainer>
inline void copyData( const InContainer& input, OutContainer& output ) {
    using OutValueType  = typename OutContainer::value_type;
    using OutScalarType = typename OutValueType::Scalar;
    std::transform( std::begin( input ),
                    std::end( input ),
                    std::back_inserter( output ),
                    []( const typename InContainer::value_type& v ) -> OutValueType {
                        return v.template cast<OutScalarType>();
                    } );
}

} // namespace internal

template <typename Container>
inline void GeometryData::setVertices( const Container& vertexList ) {
    return setAttribData( "vertex", vertexList );
}

inline Vector2uArray& GeometryData::getEdges() {
    return m_edge;
}

inline const Vector2uArray& GeometryData::getEdges() const {
    return m_edge;
}

template <typename Container>
inline void GeometryData::setEdges( const Container& edgeList ) {
    internal::copyData( edgeList, m_edge );
}

inline const VectorNuArray& GeometryData::getFaces() const {
    return m_faces;
}

inline VectorNuArray& GeometryData::getFaces() {
    return m_faces;
}

template <typename Container>
inline void GeometryData::setFaces( const Container& faceList ) {
    internal::copyData( faceList, m_faces );
}

inline VectorNuArray& GeometryData::getPolyhedra() {
    return m_polyhedron;
}

inline const VectorNuArray& GeometryData::getPolyhedra() const {
    return m_polyhedron;
}

template <typename Container>
inline void GeometryData::setPolyhedra( const Container& polyList ) {
    internal::copyData( polyList, m_polyhedron );
}

inline Vector3Array& GeometryData::getNormals() {
    return getAttribDataWithLock<Vector3Array&>( "normal" );
}

inline const Vector3Array& GeometryData::getNormals() const {
    return getAttribData<const Vector3Array&>( "normal" );
}

template <typename Container>
inline void GeometryData::setNormals( const Container& normalList ) {
    return setAttribData( "normal", normalList );
}

inline Vector3Array& GeometryData::getTangents() {
    return getAttribDataWithLock<Vector3Array&>( "tangent" );
}

inline const Vector3Array& GeometryData::getTangents() const {
    return getAttribData<const Vector3Array&>( "tangent" );
}

template <typename Container>
inline void GeometryData::setTangents( const Container& tangentList ) {
    return setAttribData( "tangent", tangentList );
}

inline Vector3Array& GeometryData::getBiTangents() {
    return getAttribDataWithLock<Vector3Array&>( "biTangent" );
}

inline const Vector3Array& GeometryData::getBiTangents() const {
    return getAttribData<const Vector3Array&>( "biTangent" );
}

template <typename Container>
inline void GeometryData::setBitangents( const Container& bitangentList ) {
    return setAttribData( "biTangent", bitangentList );
}

inline Vector3Array& GeometryData::getTexCoords() {
    return getAttribDataWithLock<Vector3Array&>( "texCoord" );
}

inline const Vector3Array& GeometryData::getTexCoords() const {
    return getAttribData<const Vector3Array&>( "texCoord" );
}

template <typename Container>
inline void GeometryData::setTextureCoordinates( const Container& texCoordList ) {
    return setAttribData( "texCoord", texCoordList );
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

inline bool GeometryData::hasVertices() const {
    return hasAttribData( "vertex" );
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
    return hasAttribData( "normal" );
}

inline bool GeometryData::hasTangents() const {
    return hasAttribData( "tangent" );
}

inline bool GeometryData::hasBiTangents() const {
    return hasAttribData( "biTangent" );
}

inline bool GeometryData::hasTextureCoordinates() const {
    return hasAttribData( "texCoord" );
}

inline bool GeometryData::hasMaterial() const {
    return m_material != nullptr;
}

const Utils::AttribManager& GeometryData::getAttribManager() const {
    return m_vertexAttribArray.vertexAttribs();
}

Utils::AttribManager& GeometryData::getAttribManager() {
    return m_vertexAttribArray.vertexAttribs();
}

template <typename Container>
inline Container& GeometryData::getAttribDataWithLock( const std::string& name ) {
    if ( name == "vertex" ) { return m_vertexAttribArray.verticesWithLock(); }
    else if ( name == "normal" ) {
        return m_vertexAttribArray.normalsWithLock();
    }
    auto h = m_vertexAttribArray.template getAttribHandle<Vector3>( name );
    if ( !m_vertexAttribArray.template isValid( h ) ) {
        h = m_vertexAttribArray.template addAttrib<Vector3>( name );
    }
    auto& attrib = m_vertexAttribArray.template getAttrib( h );
    auto& d      = attrib.getDataWithLock();
    return d;
}

template <typename Container>
inline const Container& GeometryData::getAttribData( const std::string& name ) const {
    if ( name == "vertex" ) { return m_vertexAttribArray.vertices(); }
    else if ( name == "normal" ) {
        return m_vertexAttribArray.normals();
    }
    auto h             = m_vertexAttribArray.template getAttribHandle<Vector3>( name );
    const auto& attrib = m_vertexAttribArray.template getAttrib( h );
    auto& d            = attrib.data();
    return d;
}

template <typename Container>
inline void GeometryData::setAttribData( const std::string& name,
                                         const Container& attribDataList ) {
    Utils::Attrib<Container>& c = m_vertexAttribArray.getAttribBase( name )->cast<Container>();
    auto& v                     = c.getDataWithLock();
    internal::copyData( attribDataList, v );
    attribDataUnlock( name );
}

bool GeometryData::hasAttribData( const std::string& name ) const {
    if ( name == "vertex" ) { return !m_vertexAttribArray.vertices().empty(); }
    else if ( name == "normal" ) {
        return !m_vertexAttribArray.normals().empty();
    }
    else {
        auto h = m_vertexAttribArray.getAttribHandle<Vector3>( name );
        if ( m_vertexAttribArray.isValid( h ) ) {
            return !m_vertexAttribArray.getAttrib( h ).data().empty();
        }
    }
    return false;
}

void GeometryData::attribDataUnlock( const std::string& name ) {
    if ( name == "vertex" ) { m_vertexAttribArray.verticesUnlock(); }
    else if ( name == "normal" ) {
        m_vertexAttribArray.normalsUnlock();
    }
    else {
        m_vertexAttribArray.getAttribBase( name )->unlock();
    }
}

} // namespace Asset
} // namespace Core
} // namespace Ra
