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
    auto h       = multiIndexedGeometry.getAttribHandle<Vector3>( n );
    auto& attrib = multiIndexedGeometry.getAttrib( h );
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
    return getIndexedDataWithLock<Vector2ui>( "edge" );
}

inline const Vector2uArray& GeometryData::getEdges() const {
    return getIndexedData<Vector2ui>( "edge" );
}

template <typename Container>
inline void GeometryData::setEdges( const Container& edgeList ) {
    setIndexedData( "edge", edgeList );
}

inline const VectorNuArray& GeometryData::getFaces() const {
    return getIndexedData<VectorNui>( "face" );
}

inline VectorNuArray& GeometryData::getFaces() {
    return getIndexedDataWithLock<VectorNui>( "face" );
}

template <typename Container>
inline void GeometryData::setFaces( const Container& faceList ) {
    setIndexedData( "face", faceList );
}

inline VectorNuArray& GeometryData::getPolyhedra() {
    return getIndexedDataWithLock<VectorNui>( "polyhedron" );
}

inline const VectorNuArray& GeometryData::getPolyhedra() const {
    return getIndexedData<VectorNui>( "polyhedron" );
}

template <typename Container>
inline void GeometryData::setPolyhedra( const Container& polyList ) {
    setIndexedData( "polyhedron", polyList );
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
    return !getIndexedData<Vector2ui>( "edge" ).empty();
}

inline bool GeometryData::hasFaces() const {
    return !getIndexedData<VectorNui>( "face" ).empty();
}

inline bool GeometryData::hasPolyhedra() const {
    return !getIndexedData<VectorNui>( "polyhedron" ).empty();
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
    return multiIndexedGeometry.vertexAttribs();
}

Utils::AttribManager& GeometryData::getAttribManager() {
    return multiIndexedGeometry.vertexAttribs();
}

template <typename V>
inline VectorArray<V>& GeometryData::addAttribDataWithLock( const Geometry::MeshAttrib& name ) {
    auto& n = getAttribName( name );
    auto h  = multiIndexedGeometry.getAttribHandle<V>( n );
    if ( !multiIndexedGeometry.isValid( h ) ) { h = multiIndexedGeometry.addAttrib<V>( n ); }
    auto& attrib = multiIndexedGeometry.getAttrib( h );
    auto& d      = attrib.getDataWithLock();
    return d;
}

template <typename Container>
inline void GeometryData::setAttribData( const Geometry::MeshAttrib& name,
                                         const Container& attribDataList ) {
    auto& n                     = getAttribName( name );
    Utils::Attrib<Container>& c = multiIndexedGeometry.getAttribBase( n )->cast<Container>();
    auto& v                     = c.getDataWithLock();
    internal::copyData( attribDataList, v );
    multiIndexedGeometry.getAttribBase( n )->unlock();
}

template <typename V>
inline bool GeometryData::hasAttribData( const Geometry::MeshAttrib& name ) const {
    auto& n = getAttribName( name );
    auto h  = multiIndexedGeometry.getAttribHandle<V>( n );
    if ( multiIndexedGeometry.isValid( h ) ) {
        return !multiIndexedGeometry.getAttrib( h ).data().empty();
    }
    return false;
}

inline void GeometryData::initIndexedData( const std::string& name ) {
    if ( name == "edge" ) {
        auto pil = std::make_unique<Core::Geometry::LineIndexLayer>();
        bool a   = multiIndexedGeometry.addLayer( std::move( pil ), "edge" );
        std::cout << name << a << std::endl;
    }
    else {
        auto pil1 = std::make_unique<Core::Geometry::PolyIndexLayer>();
        bool b    = multiIndexedGeometry.addLayer( std::move( pil1 ), name );
        std::cout << name << b << std::endl;
    }
}

template <typename V>
inline VectorArray<V>& GeometryData::getIndexedDataWithLock( const std::string& name ) {
    if ( name == "edge" && !multiIndexedGeometry.containsLayer(
                               { Core::Geometry::LineIndexLayer::staticSemanticName }, name ) ) {
        auto pil = std::make_unique<Core::Geometry::LineIndexLayer>();
        multiIndexedGeometry.addLayer( std::move( pil ), "edge" );
    }
    else if ( name != "edge" &&
              !multiIndexedGeometry.containsLayer(
                  { Core::Geometry::PolyIndexLayer::staticSemanticName }, name ) ) {
        auto pil1 = std::make_unique<Core::Geometry::PolyIndexLayer>();
        multiIndexedGeometry.addLayer( std::move( pil1 ), name );
    }

    if ( name == "edge" ) {
        auto& d = multiIndexedGeometry.getLayerWithLock(
            { Core::Geometry::LineIndexLayer::staticSemanticName }, name );
        auto& v    = dynamic_cast<Geometry::GeometryIndexLayer<V>&>( d );
        auto& data = v.collection();
        return data;
    }
    else {
        auto& d = multiIndexedGeometry.getLayerWithLock(
            { Core::Geometry::PolyIndexLayer::staticSemanticName }, name );
        auto& v    = dynamic_cast<Geometry::GeometryIndexLayer<V>&>( d );
        auto& data = v.collection();
        return data;
    }
}

template <typename V>
inline const VectorArray<V>& GeometryData::getIndexedData( const std::string& name ) const {
    std::set<std::string> semantics;
    for ( const auto& k : multiIndexedGeometry.layerKeys() ) {
        if ( k.second == name ) {
            semantics = k.first;
            break;
        }
    }

    const auto& d = multiIndexedGeometry.getFirstLayerOccurrence( semantics );
    const auto& v = dynamic_cast<const Geometry::GeometryIndexLayer<V>&>( d.second );
    auto& data    = v.collection();
    return data;
}

inline void GeometryData::indexedDataUnlock( const std::string& name ) {
    std::set<std::string> semantics;
    for ( const auto& k : multiIndexedGeometry.layerKeys() ) {
        if ( k.second == name ) {
            semantics = k.first;
            break;
        }
    }
    multiIndexedGeometry.unlockLayer( semantics, name );
}

template <typename V>
inline void GeometryData::setIndexedData( const std::string& name,
                                          const VectorArray<V>& attribDataList ) {
    internal::copyData( attribDataList, getIndexedDataWithLock<V>( name ) );
    indexedDataUnlock( name );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
