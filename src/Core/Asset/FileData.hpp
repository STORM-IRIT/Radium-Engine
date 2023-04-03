#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class AnimationData;
class Camera;
class GeometryData;
struct VolumeData;
class HandleData;
class LightData;

class RA_CORE_API FileData final
{
  public:
    /// CONSTRUCTOR
    FileData( const std::string& filename = "", const bool VERBOSE_MODE = false );

    FileData( FileData&& data ) = default;

    /// DESTRUCTOR
    ~FileData();

    /// FILENAME
    inline std::string getFileName() const;

    inline void setFileName( const std::string& filename );

    /// TIMING
    inline Scalar getLoadingTime() const;

    /// DATA
    inline std::vector<GeometryData*> getGeometryData() const;
    inline std::vector<VolumeData*> getVolumeData() const;
    inline std::vector<HandleData*> getHandleData() const;
    inline std::vector<AnimationData*> getAnimationData() const;
    inline std::vector<LightData*> getLightData() const;
    inline std::vector<Camera*> getCameraData() const;

    inline void setVerbose( const bool VERBOSE_MODE );

    /// QUERY
    inline bool isInitialized() const;
    inline bool isProcessed() const;
    inline bool hasGeometry() const;
    inline bool hasHandle() const;
    inline bool hasAnimation() const;
    inline bool hasLight() const;
    inline bool hasCamera() const;
    inline bool isVerbose() const;

    /// RESET
    inline void reset();

    inline void displayInfo() const;

    // TODO(Matthieu) : handle attributes in a better way than "public:"
  public:
    /// VARIABLE
    std::string m_filename;
    Scalar m_loadingTime;
    std::vector<std::unique_ptr<GeometryData>> m_geometryData;
    std::vector<std::unique_ptr<VolumeData>> m_volumeData;
    std::vector<std::unique_ptr<HandleData>> m_handleData;
    std::vector<std::unique_ptr<AnimationData>> m_animationData;
    std::vector<std::unique_ptr<LightData>> m_lightData;
    std::vector<std::unique_ptr<Camera>> m_cameraData;
    bool m_processed;
    bool m_verbose;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/AnimationData.hpp>
#include <Core/Asset/Camera.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Asset/LightData.hpp>
#include <Core/Asset/VolumeData.hpp>
#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
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
inline std::vector<GeometryData*> FileData::getGeometryData() const {
    std::vector<GeometryData*> list;
    list.reserve( m_geometryData.size() );
    for ( const auto& item : m_geometryData ) {
        list.push_back( item.get() );
    }
    return list;
}

inline std::vector<VolumeData*> FileData::getVolumeData() const {
    std::vector<VolumeData*> list;
    list.reserve( m_volumeData.size() );
    for ( const auto& item : m_volumeData ) {
        list.push_back( item.get() );
    }
    return list;
}

inline std::vector<HandleData*> FileData::getHandleData() const {
    std::vector<HandleData*> list;
    list.reserve( m_handleData.size() );
    for ( const auto& item : m_handleData ) {
        list.push_back( item.get() );
    }
    return list;
}

inline std::vector<AnimationData*> FileData::getAnimationData() const {
    std::vector<AnimationData*> list;
    list.reserve( m_animationData.size() );
    for ( const auto& item : m_animationData ) {
        list.push_back( item.get() );
    }
    return list;
}

inline std::vector<LightData*> FileData::getLightData() const {
    std::vector<LightData*> list;
    list.reserve( m_lightData.size() );
    for ( const auto& item : m_lightData ) {
        list.push_back( item.get() );
    }
    return list;
}

inline std::vector<Camera*> FileData::getCameraData() const {
    std::vector<Camera*> list;
    list.reserve( m_cameraData.size() );
    for ( const auto& item : m_cameraData ) {
        list.push_back( item.get() );
    }
    return list;
}

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

inline bool FileData::hasHandle() const {
    return ( !m_handleData.empty() );
}

inline bool FileData::hasAnimation() const {
    return ( !m_animationData.empty() );
}

inline bool FileData::hasLight() const {
    return ( !m_lightData.empty() );
}

inline bool FileData::isVerbose() const {
    return m_verbose;
}

/// RESET
inline void FileData::reset() {
    m_filename = "";
    m_geometryData.clear();
    m_handleData.clear();
    m_animationData.clear();
    m_processed = false;
}

inline void FileData::displayInfo() const {
    using namespace Core::Utils; // log
    uint64_t vtxCount = 0;
    for ( const auto& geom : m_geometryData ) {
        vtxCount += geom->getGeometry().vertices().size();
    }
    LOG( logINFO ) << "======== LOADING SUMMARY ========";
    LOG( logINFO ) << "Mesh loaded        : " << m_geometryData.size();
    LOG( logINFO ) << "Total vertex count : " << vtxCount;
    LOG( logINFO ) << "Handle loaded      : " << m_handleData.size();
    LOG( logINFO ) << "Animation loaded   : " << m_animationData.size();
    LOG( logINFO ) << "Volume loaded        : " << m_volumeData.size();
    LOG( logINFO ) << "Loading Time (sec) : " << m_loadingTime;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
