#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {
class AnimationData;
class CameraData;
class GeometryData;
class VolumeData;
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
    inline std::vector<CameraData*> getCameraData() const;

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
    std::vector<std::unique_ptr<CameraData>> m_cameraData;
    bool m_processed;
    bool m_verbose;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/FileData.inl>
