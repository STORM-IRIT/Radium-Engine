#include <Engine/Assets/FileData.hpp>

#include <Core/Log/Log.hpp>
#ifdef DEBUG_LOAD_GEOMETRY
#include <Engine/Assets/GeometryData.hpp>
#endif
#ifdef DEBUG_LOAD_HANDLE
#include <Engine/Assets/HandleData.hpp>
#endif
#ifdef DEBUG_LOAD_ANIMATION
#include <Engine/Assets/AnimationData.hpp>
#endif

namespace Ra {
namespace Asset {


/// FILENAME
inline std::string FileData::getFileName() const {
    return m_filename;
}

inline void FileData::setFileName( const std::string& filename ) {
    m_filename = filename;
}

/// TIMING
inline Scalar FileData::getLoadingTime() const {
    return m_loadingTime;
}

/// DATA
inline std::vector< GeometryData* > FileData::getGeometryData() const {
    std::vector< GeometryData* > list;
    list.reserve( m_geometryData.size() );
    for( const auto& item : m_geometryData ) {
        list.push_back( item.get() );
    }
    return list;
}

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE
inline std::vector< HandleData* > FileData::getHandleData() const {
    std::vector< HandleData* > list;
    list.reserve( m_handleData.size() );
    for( const auto& item : m_handleData ) {
        list.push_back( item.get() );
    }
    return list;
}
#endif

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
inline std::vector< AnimationData* > FileData::getAnimatinData() const {
    std::vector< AnimationData* > list;
    list.reserve( m_animationData.size() );
    for( const auto& item : m_animationData ) {
        list.push_back( item.get() );
    }
    return list;
}
#endif

inline void FileData::setVerbose( const bool VERBOSE_MODE ) {
    m_verbose = VERBOSE_MODE;
}

/// QUERY
inline bool FileData::isInitialized() const {
    return ( ( m_filename != "" ) && !m_processed );
}

inline bool FileData::isProcessed() const {
    return m_processed;
}

inline bool FileData::hasGeometry() const {
    return ( !m_geometryData.empty() );
}

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE
inline bool FileData::hasHandle() const {
    return ( !m_handleData.empty() );
}
#endif

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
inline bool FileData::hasAnimation() const {
    return ( !m_animationData.empty() );
}
#endif

inline bool FileData::isVerbose() const {
    return m_verbose;
}

/// RESET
inline void FileData::reset()
{
    m_filename = "";

    m_geometryData.clear();

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE
    m_handleData.clear();
#endif

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
    m_animationData.clear();
#endif

    m_processed = false;
}

inline void FileData::displayInfo() const {
    LOG( logDEBUG ) << "======== LOADING SUMMARY ========";
    LOG( logDEBUG ) << "Mesh loaded        : " << m_geometryData.size();

    uint64_t vtxCount = 0;
    for ( const auto& geom : m_geometryData )
    {
        vtxCount += geom->getVerticesSize();
    }
    LOG( logDEBUG ) << "Total vertex count : " << vtxCount;

#ifdef DEBUG_LOAD_HANDLE
    LOG( logDEBUG ) << "Handle loaded      : " << m_handleData.size();
#endif
#ifdef DEBUG_LOAD_ANIMATION
    LOG( logDEBUG ) << "Animation loaded   : " << m_animationData.size();
#endif
    LOG( logDEBUG ) << "Loading Time (sec) : " << m_loadingTime;
}


} // namespace Asset
} // namespace Ra

