#pragma once
#include <Core/RaCore.hpp>

#include <Core/Asset/AnimationData.hpp>
#include <Core/Asset/Camera.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Asset/LightData.hpp>
#include <Core/Asset/VolumeData.hpp>

#include <memory>
#include <string>
#include <vector>

namespace Ra {
namespace Core {
namespace Asset {

class RA_CORE_API FileData final
{
  public:
    /// CONSTRUCTOR
    explicit FileData( const std::string& filename = "", const bool VERBOSE_MODE = false ) :
        m_filename( filename ), m_verbose( VERBOSE_MODE ) {}

    FileData( FileData&& data ) = default;

    /// FILENAME
    [[nodiscard]] inline std::string getFileName() const { return m_filename; }

    inline void setFileName( const std::string& filename ) { m_filename = filename; }

    /// TIMING
    [[nodiscard]] inline Scalar getLoadingTime() const { return m_loadingTime; }

    /// DATA
    [[nodiscard]] inline const auto& getGeometryData() const { return m_geometryData; }
    [[nodiscard]] inline const auto& getVolumeData() const { return m_volumeData; }
    [[nodiscard]] inline const auto& getHandleData() const { return m_handleData; }
    [[nodiscard]] inline const auto& getAnimationData() const { return m_animationData; }
    [[nodiscard]] inline const auto& getLightData() const { return m_lightData; }
    [[nodiscard]] inline const auto& getCameraData() const { return m_cameraData; }

    inline void setVerbose( const bool VERBOSE_MODE ) { m_verbose = VERBOSE_MODE; }

    /// QUERY
    inline bool isInitialized() const { return !m_processed && m_filename != ""; }
    inline bool isProcessed() const { return m_processed; }
    inline bool hasGeometry() const { return !m_geometryData.empty(); }
    inline bool hasHandle() const { return !m_handleData.empty(); }
    inline bool hasAnimation() const { return !m_animationData.empty(); }
    inline bool hasLight() const { return !m_lightData.empty(); }
    inline bool hasCamera() const { return !m_cameraData.empty(); }
    inline bool isVerbose() const { return m_verbose; }

    /// RESET
    inline void reset();

    void displayInfo() const;

    // TODO(Matthieu) : handle attributes in a better way than "public:"
  public:
    /// VARIABLE
    std::string m_filename;
    Scalar m_loadingTime { 0_ra };
    std::vector<std::unique_ptr<GeometryData>> m_geometryData;
    std::vector<std::unique_ptr<VolumeData>> m_volumeData;
    std::vector<std::unique_ptr<HandleData>> m_handleData;
    std::vector<std::unique_ptr<AnimationData>> m_animationData;
    std::vector<std::unique_ptr<LightData>> m_lightData;
    std::vector<std::unique_ptr<Camera>> m_cameraData;
    bool m_processed { false };
    bool m_verbose { false };
};

/// RESET
inline void FileData::reset() {
    m_filename = "";
    m_geometryData.clear();
    m_volumeData.clear();
    m_handleData.clear();
    m_animationData.clear();
    m_lightData.clear();
    m_cameraData.clear();
    m_processed = false;
}

} // namespace Asset
} // namespace Core
} // namespace Ra
