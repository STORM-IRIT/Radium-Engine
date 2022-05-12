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
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_POSITION ).getDataWithLock();
}

inline const Vector3Array& GeometryData::getVertices() const {
    return m_multiIndexedGeometry.vertices();
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
    return setAttribData( Geometry::MeshAttrib::VERTEX_POSITION, vertexList );
}

inline Vector2uArray& GeometryData::getEdges() {
    return findIndexedDataWithLock<Vector2ui>( "in_edge" );
}

inline const Vector2uArray& GeometryData::getEdges() const {
    return getIndexedData<Vector2ui>( "in_edge" );
}

template <typename Container>
inline void GeometryData::setEdges( const Container& edgeList ) {
    setIndexedData( GeometryType::LINE_MESH, edgeList, "in_edge" );
}

inline const VectorNuArray& GeometryData::getFaces() const {
    return getIndexedData<VectorNui>( "in_face" );
}

inline VectorNuArray& GeometryData::getFaces() {
    return findIndexedDataWithLock<VectorNui>( "in_face" );
}

template <typename Container>
inline void GeometryData::setFaces( const Container& faceList ) {
    setIndexedData( GeometryType::POLY_MESH, faceList, "in_face" );
}

inline VectorNuArray& GeometryData::getPolyhedra() {
    return findIndexedDataWithLock<VectorNui>( "in_polyhedron" );
}

inline const VectorNuArray& GeometryData::getPolyhedra() const {
    return getIndexedData<VectorNui>( "in_polyhedron" );
}

template <typename Container>
inline void GeometryData::setPolyhedra( const Container& polyList ) {
    setIndexedData( GeometryType::POLY_MESH, polyList, "in_polyhedron" );
}

inline Vector3Array& GeometryData::getNormals() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_NORMAL ).getDataWithLock();
}

inline const Vector3Array& GeometryData::getNormals() const {
    return m_multiIndexedGeometry.normals();
}

template <typename Container>
inline void GeometryData::setNormals( const Container& normalList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_NORMAL, normalList );
}

inline Vector3Array& GeometryData::getTangents() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TANGENT ).getDataWithLock();
}

inline const Vector3Array& GeometryData::getTangents() const {
    auto& name = getAttribName( Geometry::MeshAttrib::VERTEX_TANGENT );
    auto h     = m_multiIndexedGeometry.getAttribHandle<Vector3>( name );
    return m_multiIndexedGeometry.getAttrib( h ).data();
}

template <typename Container>
inline void GeometryData::setTangents( const Container& tangentList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_TANGENT, tangentList );
}

inline Vector3Array& GeometryData::getBiTangents() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_BITANGENT ).getDataWithLock();
}

inline const Vector3Array& GeometryData::getBiTangents() const {
    auto& name = getAttribName( Geometry::MeshAttrib::VERTEX_BITANGENT );
    auto h     = m_multiIndexedGeometry.getAttribHandle<Vector3>( name );
    return m_multiIndexedGeometry.getAttrib( h ).data();
}

template <typename Container>
inline void GeometryData::setBitangents( const Container& bitangentList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_BITANGENT, bitangentList );
}

inline Vector3Array& GeometryData::getTexCoords() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TEXCOORD ).getDataWithLock();
}

inline const Vector3Array& GeometryData::getTexCoords() const {
    auto& name = getAttribName( Geometry::MeshAttrib::VERTEX_TEXCOORD );
    auto h     = m_multiIndexedGeometry.getAttribHandle<Vector3>( name );
    return m_multiIndexedGeometry.getAttrib( h ).data();
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
    return m_multiIndexedGeometry.vertexAttribs().contains( "in_position" );
}

inline bool GeometryData::hasEdges() const {
    return m_multiIndexedGeometry.containsLayer( { Geometry::LineIndexLayer::staticSemanticName },
                                                 "in_edge" );
}

inline bool GeometryData::hasFaces() const {
    return m_multiIndexedGeometry.containsLayer( { Geometry::PolyIndexLayer::staticSemanticName },
                                                 "in_face" );
}

inline bool GeometryData::hasPolyhedra() const {
    return m_multiIndexedGeometry.containsLayer( { Geometry::PolyIndexLayer::staticSemanticName },
                                                 "in_polyhedron" );
}

inline bool GeometryData::hasNormals() const {
    return m_multiIndexedGeometry.vertexAttribs().contains( "in_normal" );
}

inline bool GeometryData::hasTangents() const {
    return m_multiIndexedGeometry.vertexAttribs().contains( "in_tangent" );
}

inline bool GeometryData::hasBiTangents() const {
    return m_multiIndexedGeometry.vertexAttribs().contains( "in_bitangent" );
}

inline bool GeometryData::hasTextureCoordinates() const {
    return m_multiIndexedGeometry.vertexAttribs().contains( "in_texcoord" );
}

inline bool GeometryData::hasMaterial() const {
    return m_material != nullptr;
}

const Geometry::MultiIndexedGeometry& GeometryData::getMultiIndexedGeometry() const {
    return m_multiIndexedGeometry;
}

Geometry::MultiIndexedGeometry& GeometryData::getMultiIndexedGeometry() {
    return m_multiIndexedGeometry;
}

const Utils::AttribManager& GeometryData::getAttribManager() const {
    return m_multiIndexedGeometry.vertexAttribs();
}

Utils::AttribManager& GeometryData::getAttribManager() {
    return m_multiIndexedGeometry.vertexAttribs();
}

template <typename T>
inline Utils::Attrib<T>& GeometryData::getAttrib( const Geometry::MeshAttrib& name ) {
    const auto& n = getAttribName( name );
    auto h        = m_multiIndexedGeometry.addAttrib<T>( n );
    auto& attrib  = m_multiIndexedGeometry.getAttrib( h );
    return attrib;
}

template <typename V>
inline void GeometryData::setAttribData( const Geometry::MeshAttrib& name,
                                         const VectorArray<V>& attribDataList ) {
    auto& attrib = getAttrib<V>( name );
    auto& data   = attrib.getDataWithLock();
    internal::copyData( attribDataList, data );
    attrib.unlock();
}

template <typename V>
inline VectorArray<V>&
GeometryData::getDataFromLayerBase( Geometry::GeometryIndexLayerBase& geomBase ) {
    auto& v    = dynamic_cast<Geometry::GeometryIndexLayer<V>&>( geomBase );
    auto& data = v.collection();
    return data;
}

template <typename V>
inline const VectorArray<V>&
GeometryData::getDataFromLayerBase( const Geometry::GeometryIndexLayerBase& geomBase ) const {
    const auto& v    = dynamic_cast<const Geometry::GeometryIndexLayer<V>&>( geomBase );
    const auto& data = v.collection();
    return data;
}

template <typename L>
inline Geometry::GeometryIndexLayerBase&
GeometryData::getLayerBaseWithLock( const bool& firstOccurrence, const std::string& name ) {
    auto& geomBase =
        ( firstOccurrence )
            ? m_multiIndexedGeometry.getFirstLayerOccurrenceWithLock( L::staticSemanticName ).second
            : m_multiIndexedGeometry.getLayerWithLock( { L::staticSemanticName }, name );
    return geomBase;
}

template <typename L>
inline const Geometry::GeometryIndexLayerBase&
GeometryData::getLayerBase( const bool& firstOccurrence, const std::string& name ) const {
    const auto& geomBase =
        ( firstOccurrence )
            ? m_multiIndexedGeometry.getFirstLayerOccurrence( L::staticSemanticName ).second
            : m_multiIndexedGeometry.getLayer( { L::staticSemanticName }, name );
    return geomBase;
}

template <typename V, typename L>
inline VectorArray<V>& GeometryData::getIndexedDataWithLock( const bool& firstOccurrence,
                                                             const std::string& name ) {
    if ( m_multiIndexedGeometry.containsLayer( { L::staticSemanticName }, name ) ) {
        auto& geomBaseFound = getLayerBaseWithLock<L>( firstOccurrence, name );
        return getDataFromLayerBase<V>( geomBaseFound );
    }
    else {
        auto pil = std::make_unique<L>();
        auto& geomBaseCreated =
            m_multiIndexedGeometry.addLayer( std::move( pil ), true, name ).second;
        return getDataFromLayerBase<V>( geomBaseCreated );
    }
}

template <typename V>
inline VectorArray<V>& GeometryData::findIndexedDataWithLock( const std::string& name,
                                                              const bool& firstOccurrence ) {
    if ( std::is_same<V, Vector2ui>::value ) {
        return getIndexedDataWithLock<V, Geometry::LineIndexLayer>( firstOccurrence, name );
    }
    /* else if (std::is_same<V, Vector3ui>::value){
        return getIndexedDataWithLock<V, Geometry::TriangleIndexLayer>( firstOccurrence, name );
    }
    else if (std::is_same<V, Vector4ui>::value){
        return getIndexedDataWithLock<V, Geometry::QuadIndexLayer>( firstOccurrence, name );
    } */
    else {
        return getIndexedDataWithLock<V, Geometry::PolyIndexLayer>( firstOccurrence, name );
    }
}

template <typename V>
inline const VectorArray<V>& GeometryData::getIndexedData( const std::string& name,
                                                           const bool& firstOccurrence ) const {
    if ( std::is_same<V, Vector2ui>::value ) {
        auto& geomBase = getLayerBase<Geometry::LineIndexLayer>( firstOccurrence, name );
        return getDataFromLayerBase<V>( geomBase );
    }
    /*else if (std::is_same<V, Vector3ui>::value){
        auto& geomBase = getLayerBase<Geometry::TriangleIndexLayer>( firstOccurrence, name );
        return getDataFromLayerBase<V>( geomBase );
    }
    else if (std::is_same<V, Vector4ui>::value){
        auto& geomBase = getLayerBase<Geometry::QuadIndexLayer>( firstOccurrence, name );
        return getDataFromLayerBase<V>( geomBase );
    }*/
    else {
        auto& geomBase = getLayerBase<Geometry::PolyIndexLayer>( firstOccurrence, name );
        return getDataFromLayerBase<V>( geomBase );
    }
}

inline void GeometryData::indexedDataUnlock( const GeometryType& type, const std::string& name ) {
    switch ( type ) {
    case GeometryType::LINE_MESH:
        m_multiIndexedGeometry.unlockLayer(
            { { Core::Geometry::LineIndexLayer::staticSemanticName }, name } );
        break;
    case GeometryType::TRI_MESH:
        // m_multiIndexedGeometry.unlockLayer( {{
        // Core::Geometry::TriangleIndexLayer::staticSemanticName }, name }); break;
    case GeometryType::QUAD_MESH:
        // m_multiIndexedGeometry.unlockLayer( {{ Core::Geometry::QuadIndexLayer::staticSemanticName
        // }, name }); break;
    default:
        m_multiIndexedGeometry.unlockLayer(
            { { Core::Geometry::PolyIndexLayer::staticSemanticName }, name } );
        break;
    }
}

template <typename V>
inline void GeometryData::setIndexedData( const GeometryType& type,
                                          const VectorArray<V>& indexedDataList,
                                          const std::string& name ) {
    internal::copyData( indexedDataList, findIndexedDataWithLock<V>( name ) );
    indexedDataUnlock( type, name );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
