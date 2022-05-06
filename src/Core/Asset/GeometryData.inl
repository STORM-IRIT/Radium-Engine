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
    return addIndexedDataWithLock<Vector2ui>( "in_edge" );
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
    return addIndexedDataWithLock<VectorNui>( "in_face" );
}

template <typename Container>
inline void GeometryData::setFaces( const Container& faceList ) {
    setIndexedData( GeometryType::POLY_MESH, faceList, "in_face" );
}

inline VectorNuArray& GeometryData::getPolyhedra() {
    return addIndexedDataWithLock<VectorNui>( "in_polyhedron" );
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

template <typename Container>
inline void GeometryData::setNormals( const Container& normalList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_NORMAL, normalList );
}

inline Vector3Array& GeometryData::getTangents() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TANGENT ).getDataWithLock();
}

template <typename Container>
inline void GeometryData::setTangents( const Container& tangentList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_TANGENT, tangentList );
}

inline Vector3Array& GeometryData::getBiTangents() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_BITANGENT ).getDataWithLock();
}

template <typename Container>
inline void GeometryData::setBitangents( const Container& bitangentList ) {
    return setAttribData( Geometry::MeshAttrib::VERTEX_BITANGENT, bitangentList );
}

inline Vector3Array& GeometryData::getTexCoords() {
    return getAttrib<Vector3>( Geometry::MeshAttrib::VERTEX_TEXCOORD ).getDataWithLock();
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
    return !getIndexedData<Vector2ui>( "in_edge" ).empty();
}

inline bool GeometryData::hasFaces() const {
    return !getIndexedData<VectorNui>( "in_face" ).empty();
}

inline bool GeometryData::hasPolyhedra() const {
    return !getIndexedData<VectorNui>( "in_polyhedron" ).empty();
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

inline Geometry::MultiIndexedGeometry& GeometryData::getMultiIndexedGeometry() {
    return m_multiIndexedGeometry;
}

Utils::AttribManager& GeometryData::getAttribManager() {
    return m_multiIndexedGeometry.vertexAttribs();
}

template <typename T>
inline Utils::Attrib<T>& GeometryData::getAttrib( const Geometry::MeshAttrib& name ) {
    const auto& n = getAttribName( name );
    Utils::AttribHandle<T> h;
    if ( m_multiIndexedGeometry.vertexAttribs().contains( n ) ) {
        h = m_multiIndexedGeometry.getAttribHandle<T>( n );
    }
    else {
        h = m_multiIndexedGeometry.addAttrib<T>( n );
    }
    auto& attrib = m_multiIndexedGeometry.getAttrib( h );
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
inline bool GeometryData::hasAttribData( const Geometry::MeshAttrib& name ) const {
    auto& n = getAttribName( name );
    auto h  = m_multiIndexedGeometry.getAttribHandle<V>( n );
    if ( m_multiIndexedGeometry.isValid( h ) ) {
        return !m_multiIndexedGeometry.getAttrib( h ).data().empty();
    }
    return false;
}

template <typename V, typename L>
inline VectorArray<V>& GeometryData::getIndexedDataWithLock( const bool& firstOccurrence,
                                                             const std::string& name ) {
    try {
        auto& geomBase =
            ( firstOccurrence )
                ? m_multiIndexedGeometry.getFirstLayerOccurrenceWithLock( L::staticSemanticName )
                      .second
                : m_multiIndexedGeometry.getLayerWithLock( { L::staticSemanticName }, name );
        auto& v    = dynamic_cast<Geometry::GeometryIndexLayer<V>&>( geomBase );
        auto& data = v.collection();
        return data;
    }
    catch ( const std::out_of_range& e ) {
        auto pil = std::make_unique<L>();
        m_multiIndexedGeometry.addLayer( std::move( pil ), name );
        return getIndexedDataWithLock<V, L>( firstOccurrence, name );
    }
}

template <typename V>
inline VectorArray<V>& GeometryData::addIndexedDataWithLock( const std::string& name,
                                                             const bool& firstOccurrence ) {
    if ( std::is_same<V, Vector2ui>::value ) {
        return getIndexedDataWithLock<V, Geometry::LineIndexLayer>( firstOccurrence, name );
    }
    /*else if (std::is_same<V, Vector3ui>::value){
        return getIndexedDataWithLock<V, Geometry::TriangleIndexLayer>( firstOccurrence, name );
    }
    else if (std::is_same<V, Vector4ui>::value){
        return getIndexedDataWithLock<V, Geometry::QuadIndexLayer>( firstOccurrence, name );
    }*/
    else {
        return getIndexedDataWithLock<V, Geometry::PolyIndexLayer>( firstOccurrence, name );
    }
}

template <typename V, typename L>
inline const VectorArray<V>& GeometryData::getIndexedData( const bool& firstOccurrence,
                                                           const std::string& name ) const {
    auto& g    = ( firstOccurrence )
                     ? m_multiIndexedGeometry.getFirstLayerOccurrence( L::staticSemanticName ).second
                     : m_multiIndexedGeometry.getLayer( { L::staticSemanticName }, name );
    auto& v    = dynamic_cast<const Geometry::GeometryIndexLayer<V>&>( g );
    auto& data = v.collection();
    return data;
}

template <typename V>
inline const VectorArray<V>& GeometryData::getIndexedData( const std::string& name,
                                                           const bool& firstOccurrence ) const {
    if ( std::is_same<V, Vector2ui>::value ) {
        return getIndexedData<V, Geometry::LineIndexLayer>( firstOccurrence, name );
    }
    /*else if (std::is_same<V, Vector3ui>::value){
        return getIndexedData<V, Geometry::TriangleIndexLayer>( firstOccurrence, name );
    }
    else if (std::is_same<V, Vector4ui>::value){
        return getIndexedData<V, Geometry::QuadIndexLayer>( firstOccurrence, name );
    }*/
    else {
        return getIndexedData<V, Geometry::PolyIndexLayer>( firstOccurrence, name );
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
    internal::copyData( indexedDataList, addIndexedDataWithLock<V>( name ) );
    indexedDataUnlock( type, name );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
