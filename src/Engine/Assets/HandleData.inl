#include <Engine/Assets/HandleData.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Asset {

/// NAME
/*inline std::string HandleData::getName() const {
    return m_name;
}*/

/// TYPE
inline HandleData::HandleType HandleData::getType() const {
    return m_type;
}

/// FRAME
inline Core::Transform HandleData::getFrame() const {
    return m_frame;
}

/// VERTEX SIZE
inline uint HandleData::getVertexSize() const {
    return m_vertexSize;
}

/// DATA
inline uint HandleData::getComponentDataSize() const {
    return m_component.size();
}

inline Core::AlignedStdVector<HandleComponentData> HandleData::getComponentData() const {
    return m_component;
}

inline HandleComponentData HandleData::getComponent( const uint i ) const {
    return m_component.at( i );
}

inline Core::AlignedStdVector<Core::Vector2i> HandleData::getEdgeData() const {
    return m_edge;
}

inline Core::AlignedStdVector<Core::VectorNi> HandleData::getFaceData() const {
    return m_face;
}

inline void HandleData::recomputeAllIndices() {
    m_nameTable.clear();
    const uint size = getComponentDataSize();
    for( uint i = 0; i < size; ++i ) {
        m_nameTable[m_component[i].m_name] = i;
    }
}

/// QUERY
inline bool HandleData::isPointCloud() const {
    return ( m_type == POINT_CLOUD );
}

inline bool HandleData::isSkeleton() const {
    return ( m_type == SKELETON );
}

inline bool HandleData::isCage() const {
    return ( m_type == CAGE );
}

inline bool HandleData::hasComponents() const {
    return !m_component.empty();
}

inline bool HandleData::hasEdges() const {
    return !m_edge.empty();
}

inline bool HandleData::hasFaces() const {
    return !m_face.empty();
}

inline bool HandleData::needsEndNodes() const {
    return m_endNode;
}

inline int HandleData::getIndexOf( const std::string& name ) const {
    auto it = m_nameTable.find( name );
    if( it == m_nameTable.end() ) {
        return -1;
    }
    return it->second;
}

/// DEBUG
inline void HandleData::displayInfo() const {
    std::string type;
    switch( m_type ) {
        case UNKNOWN     : type = "UNKNOWN";     break;
        case POINT_CLOUD : type = "POINT CLOUD"; break;
        case SKELETON    : type = "SKELETON";    break;
        case CAGE        : type = "CAGE";        break;
    }
    LOG( logDEBUG ) << "======== HANDLE INFO ========";
    LOG( logDEBUG ) << " Name            : " << m_name;
    LOG( logDEBUG ) << " Type            : " << type;
    LOG( logDEBUG ) << " Element #       : " << m_component.size();
    LOG( logDEBUG ) << " Edge #          : " << m_edge.size();
    LOG( logDEBUG ) << " Face #          : " << m_face.size();
    LOG( logDEBUG ) << " Need EndNodes ? : " << ( ( m_endNode ) ? "YES" : "NO" );
}

/// NAME
inline void HandleData::setName( const std::string& name ) {
    m_name = name;
}

/// TYPE
inline void HandleData::setType( const HandleType& type ) {
    m_type = type;
}

/// FRAME
inline void HandleData::setFrame( const Core::Transform& frame ) {
    m_frame = frame;
}

/// TABLE
inline void HandleData::setNameTable( const std::map< std::string, uint >& nameTable ) {
    m_nameTable = nameTable;
}

/// COMPONENT
inline void HandleData::setComponents( const Core::AlignedStdVector< HandleComponentData >& components ) {
    const uint size = components.size();
    m_component.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_component[i] = components[i];
    }
}

/// EDGE
inline void HandleData::setEdges( const Core::AlignedStdVector< Core::Vector2i >& edgeList ) {
    const uint size = edgeList.size();
    m_edge.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_edge[i] = edgeList[i];
    }
}

/// FACE
inline void HandleData::setFaces( const Core::AlignedStdVector< Core::VectorNi >& faceList ) {
    const uint size = faceList.size();
    m_face.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_face[i] = faceList[i];
    }
}


} // namespace Asset
} // namespace Ra
