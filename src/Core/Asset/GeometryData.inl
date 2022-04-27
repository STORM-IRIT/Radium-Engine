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
    auto& n      = getAttribName( Geometry::MeshAttrib::VERTEX_POSITION );
    auto h       = m_multiIndexedGeometry.getAttribHandle<Vector3>( n );
    auto& attrib = m_multiIndexedGeometry.getAttrib( h );
    return attrib.data().size();
}

inline Vector3Array& GeometryData::getVertices() {
    return addAttribDataWithLock<Vector3>( Geometry::MeshAttrib::VERTEX_POSITION );
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
    return addIndexedDataWithLock<Vector2ui>( GeometryType::LINE_MESH, "in_edge" );
}

inline const Vector2uArray& GeometryData::getEdges() const {
    return getIndexedData<Vector2ui>( GeometryType::LINE_MESH, "in_edge" );
}

template <typename Container>
inline void GeometryData::setEdges( const Container& edgeList ) {
    setIndexedData( GeometryType::LINE_MESH, edgeList, "in_edge" );
}

inline const VectorNuArray& GeometryData::getFaces() const {
    return getIndexedData<VectorNui>( GeometryType::POLY_MESH, "in_face" );
}

inline VectorNuArray& GeometryData::getFaces() {
    return addIndexedDataWithLock<VectorNui>( GeometryType::POLY_MESH, "in_face" );
}

template <typename Container>
inline void GeometryData::setFaces( const Container& faceList ) {
    setIndexedData( GeometryType::POLY_MESH, faceList, "in_face" );
}

inline VectorNuArray& GeometryData::getPolyhedra() {
    return addIndexedDataWithLock<VectorNui>( GeometryType::POLY_MESH, "in_polyhedron" );
}

inline const VectorNuArray& GeometryData::getPolyhedra() const {
    return getIndexedData<VectorNui>( GeometryType::POLY_MESH, "in_polyhedron" );
}

template <typename Container>
inline void GeometryData::setPolyhedra( const Container& polyList ) {
    setIndexedData( GeometryType::POLY_MESH, polyList, "in_polyhedron" );
}

inline Vector3Array& GeometryData::getNormals() {
    return addAttribDataWithLock<Vector3>( Geometry::MeshAttrib::VERTEX_NORMAL );
}

template <typename Container>
inline void GeometryData::setNormals( const Container& normalList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_NORMAL, normalList );
}

inline Vector3Array& GeometryData::getTangents() {
    return addAttribDataWithLock<Vector3>( Geometry::MeshAttrib::VERTEX_TANGENT );
}

template <typename Container>
inline void GeometryData::setTangents( const Container& tangentList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_TANGENT, tangentList );
}

inline Vector3Array& GeometryData::getBiTangents() {
    return addAttribDataWithLock<Vector3>( Geometry::MeshAttrib::VERTEX_BITANGENT );
}

template <typename Container>
inline void GeometryData::setBitangents( const Container& bitangentList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_BITANGENT, bitangentList );
}

inline Vector3Array& GeometryData::getTexCoords() {
    return addAttribDataWithLock<Vector3>( Geometry::MeshAttrib::VERTEX_TEXCOORD );
}

template <typename Container>
inline void GeometryData::setTextureCoordinates( const Container& texCoordList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_TEXCOORD, texCoordList );
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
    return hasAttribData<Vector3>( Geometry::MeshAttrib::VERTEX_POSITION );
}

inline bool GeometryData::hasEdges() const {
    return !getIndexedData<Vector2ui>( GeometryType::LINE_MESH, "in_edge" ).empty();
}

inline bool GeometryData::hasFaces() const {
    return !getIndexedData<VectorNui>( GeometryType::POLY_MESH, "in_face" ).empty();
}

inline bool GeometryData::hasPolyhedra() const {
    return !getIndexedData<VectorNui>( GeometryType::POLY_MESH, "in_polyhedron" ).empty();
}

inline bool GeometryData::hasNormals() const {
    return hasAttribData<Vector3>( Geometry::MeshAttrib::VERTEX_NORMAL );
}

inline bool GeometryData::hasTangents() const {
    return hasAttribData<Vector3>( Geometry::MeshAttrib::VERTEX_TANGENT );
}

inline bool GeometryData::hasBiTangents() const {
    return hasAttribData<Vector3>( Geometry::MeshAttrib::VERTEX_BITANGENT );
}

inline bool GeometryData::hasTextureCoordinates() const {
    return hasAttribData<Vector3>( Geometry::MeshAttrib::VERTEX_TEXCOORD );
}

inline bool GeometryData::hasMaterial() const {
    return m_material != nullptr;
}

const Utils::AttribManager& GeometryData::getAttribManager() const {
    return m_multiIndexedGeometry.vertexAttribs();
}

Utils::AttribManager& GeometryData::getAttribManager() {
    return m_multiIndexedGeometry.vertexAttribs();
}

template <typename V>
inline VectorArray<V>& GeometryData::addAttribDataWithLock( const Geometry::MeshAttrib& name ) {
    auto& n = getAttribName( name );
    auto h  = m_multiIndexedGeometry.getAttribHandle<V>( n );
    if ( !m_multiIndexedGeometry.isValid( h ) ) { h = m_multiIndexedGeometry.addAttrib<V>( n ); }
    auto& attrib = m_multiIndexedGeometry.getAttrib( h );
    auto& d      = attrib.getDataWithLock();
    return d;
}

template <typename Container>
inline void GeometryData::setAttribData( const Geometry::MeshAttrib& name,
                                         const Container& attribDataList ) {
    auto& n                     = getAttribName( name );
    Utils::Attrib<Container>& c = m_multiIndexedGeometry.getAttribBase( n )->cast<Container>();
    auto& v                     = c.getDataWithLock();
    internal::copyData( attribDataList, v );
    m_multiIndexedGeometry.getAttribBase( n )->unlock();
}

template <typename V>
inline bool GeometryData::hasAttribData( const Geometry::MeshAttrib& name ) const {
    auto& n = getAttribName( name );
    auto h  = m_multiIndexedGeometry.getAttribHandle<V>( n );
    if ( m_multiIndexedGeometry.isValid( h ) ) {
        return !m_multiIndexedGeometry.getAttrib( h ).data().empty();
    }
    return false;
}

template <typename L>
inline bool GeometryData::initLayer( const std::string& name ) {
    if ( !m_multiIndexedGeometry.containsLayer( { L::staticSemanticName }, name ) ) {
        auto pil = std::make_unique<L>();
        return m_multiIndexedGeometry.addLayer( std::move( pil ), name );
    }
    return false;
}

template <typename V, typename L>
inline VectorArray<V>& GeometryData::getIndexedDataWithLock( const std::string& name ) {
    auto& d    = m_multiIndexedGeometry.getLayerWithLock( { L::staticSemanticName }, name );
    auto& v    = dynamic_cast<Geometry::GeometryIndexLayer<V>&>( d );
    auto& data = v.collection();
    return data;
}

template <typename V>
inline VectorArray<V>& GeometryData::addIndexedDataWithLock( const GeometryType& type,
                                                             const std::string& name ) {
    switch ( type ) {
    case GeometryType::LINE_MESH:
        initLayer<Geometry::LineIndexLayer>( name );
        return getIndexedDataWithLock<V, Geometry::LineIndexLayer>( name );
    case GeometryType::TRI_MESH:
        // initLayer<Geometry::TriangleIndexLayer>( name );
        // return getIndexedDataWithLock<V, Geometry::TriangleIndexLayer>( name );
    case GeometryType::QUAD_MESH:
        // initLayer<Geometry::QuadIndexLayer>( name );
        // return getIndexedDataWithLock<V, Geometry::QuadIndexLayer>( name );
    default:
        initLayer<Geometry::PolyIndexLayer>( name );
        return getIndexedDataWithLock<V, Geometry::PolyIndexLayer>( name );
    }
}

template <typename V, typename L>
inline const VectorArray<V>& GeometryData::getIndexedData( const std::string& name ) const {
    auto& d    = m_multiIndexedGeometry.getLayer( { L::staticSemanticName }, name );
    auto& v    = dynamic_cast<const Geometry::GeometryIndexLayer<V>&>( d );
    auto& data = v.collection();
    return data;
}

template <typename V>
inline const VectorArray<V>& GeometryData::getIndexedData( const GeometryType& type,
                                                           const std::string& name ) const {
    switch ( type ) {
    case GeometryType::LINE_MESH:
        return getIndexedData<V, Geometry::LineIndexLayer>( name );
    case GeometryType::TRI_MESH:
        // return getIndexedDataWithLock<V, Geometry::TriangleIndexLayer>( name );
    case GeometryType::QUAD_MESH:
        // return getIndexedDataWithLock<V, Geometry::QuadIndexLayer>( name );
    default:
        return getIndexedData<V, Geometry::PolyIndexLayer>( name );
    }
}

inline void GeometryData::indexedDataUnlock( const GeometryType& type, const std::string& name ) {
    switch ( type ) {
    case GeometryType::LINE_MESH:
        m_multiIndexedGeometry.unlockLayer( { Core::Geometry::LineIndexLayer::staticSemanticName },
                                            name );
        break;
    case GeometryType::TRI_MESH:
        // m_multiIndexedGeometry.unlockLayer( {
        // Core::Geometry::TriangleIndexLayer::staticSemanticName }, name ); break;
    case GeometryType::QUAD_MESH:
        // m_multiIndexedGeometry.unlockLayer( { Core::Geometry::QuadIndexLayer::staticSemanticName
        // }, name ); break;
    default:
        m_multiIndexedGeometry.unlockLayer( { Core::Geometry::PolyIndexLayer::staticSemanticName },
                                            name );
        break;
    }
}

template <typename V>
inline void GeometryData::setIndexedData( const GeometryType& type,
                                          const VectorArray<V>& indexedDataList,
                                          const std::string& name ) {
    internal::copyData( indexedDataList, addIndexedDataWithLock<V>( type, name ) );
    indexedDataUnlock( type, name );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
