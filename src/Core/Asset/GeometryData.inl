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
    return !getIndexedData<Vector2ui>( "edge" ).empty();
}

inline bool GeometryData::hasFaces() const {
    return !getIndexedData<VectorNui>( "face" ).empty();
}

inline bool GeometryData::hasPolyhedra() const {
    return !getIndexedData<VectorNui>( "polyhedron" ).empty();
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
    return multiIndexedGeometry.vertexAttribs();
}

Utils::AttribManager& GeometryData::getAttribManager() {
    return multiIndexedGeometry.vertexAttribs();
}

template <typename Container>
inline Container& GeometryData::getAttribDataWithLock( const std::string& name ) {
    if ( name == "vertex" ) { return multiIndexedGeometry.verticesWithLock(); }
    else if ( name == "normal" ) {
        return multiIndexedGeometry.normalsWithLock();
    }
    auto h = multiIndexedGeometry.template getAttribHandle<Vector3>( name );
    if ( !multiIndexedGeometry.template isValid( h ) ) {
        h = multiIndexedGeometry.template addAttrib<Vector3>( name );
    }
    auto& attrib = multiIndexedGeometry.template getAttrib( h );
    auto& d      = attrib.getDataWithLock();
    return d;
}

template <typename Container>
inline const Container& GeometryData::getAttribData( const std::string& name ) const {
    if ( name == "vertex" ) { return multiIndexedGeometry.vertices(); }
    else if ( name == "normal" ) {
        return multiIndexedGeometry.normals();
    }
    auto h             = multiIndexedGeometry.template getAttribHandle<Vector3>( name );
    const auto& attrib = multiIndexedGeometry.template getAttrib( h );
    auto& d            = attrib.data();
    return d;
}

template <typename Container>
inline void GeometryData::setAttribData( const std::string& name,
                                         const Container& attribDataList ) {
    Utils::Attrib<Container>& c = multiIndexedGeometry.getAttribBase( name )->cast<Container>();
    auto& v                     = c.getDataWithLock();
    internal::copyData( attribDataList, v );
    attribDataUnlock( name );
}

bool GeometryData::hasAttribData( const std::string& name ) const {
    if ( name == "vertex" ) { return !multiIndexedGeometry.vertices().empty(); }
    else if ( name == "normal" ) {
        return !multiIndexedGeometry.normals().empty();
    }
    else {
        auto h = multiIndexedGeometry.getAttribHandle<Vector3>( name );
        if ( multiIndexedGeometry.isValid( h ) ) {
            return !multiIndexedGeometry.getAttrib( h ).data().empty();
        }
    }
    return false;
}

void GeometryData::attribDataUnlock( const std::string& name ) {
    if ( name == "vertex" ) { multiIndexedGeometry.verticesUnlock(); }
    else if ( name == "normal" ) {
        multiIndexedGeometry.normalsUnlock();
    }
    else {
        multiIndexedGeometry.getAttribBase( name )->unlock();
    }
}

inline void GeometryData::initIndexedData( const std::string& name ) {
    if ( name == "edge" ) {
        auto pil = std::make_unique<Core::Geometry::LineIndexLayer>();
        multiIndexedGeometry.addLayer( std::move( pil ), "edge" );
    }
    else {
        auto pil1 = std::make_unique<Core::Geometry::PolyIndexLayer>();
        multiIndexedGeometry.addLayer( std::move( pil1 ), name );
    }
}

template <typename V>
inline VectorArray<V>& GeometryData::getIndexedDataWithLock( const std::string& name ) {
    initIndexedData( name );
    std::set<std::string> semantics;
    for ( const auto& k : multiIndexedGeometry.layerKeys() ) {
        if ( k.second == name ) {
            semantics = k.first;
            std::cout << name << std::endl;
            break;
        }
    }

    auto& d = multiIndexedGeometry.getLayerWithLock( semantics, name );

    auto& v = dynamic_cast<Geometry::GeometryIndexLayer<V>&>( d );
    // if ( v.collection().empty() ) { v.collection() = VectorArray<V>(); }
    auto& data = v.collection();
    return data;
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
