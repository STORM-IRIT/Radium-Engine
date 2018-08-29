#ifndef RADIUMENGINE_FILE_DATA_HPP
#define RADIUMENGINE_FILE_DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include <Core/RaCore.hpp>

namespace Ra {
namespace Asset {
class GeometryData;
class HandleData;
class AnimationData;
class LightData;

/// The FileData class stores all the Data extracted from a loaded file.
class RA_CORE_API FileData final {
  public:
    FileData( const std::string& filename = "", const bool VERBOSE_MODE = false );

    FileData( FileData&& data ) = default;

    ~FileData();

    /// Return the filename.
    inline std::string getFileName() const;

    /// Set the filename.
    inline void setFileName( const std::string& filename );

    /// Return the loading time.
    inline Scalar getLoadingTime() const;

    /// Return the list of loaded GeometryData.
    inline std::vector<GeometryData*> getGeometryData() const;

    /// Return the list of loaded HandleData.
    inline std::vector<HandleData*> getHandleData() const;

    /// Return the list of loaded AnimationData.
    inline std::vector<AnimationData*> getAnimationData() const;

    /// Return the list of loaded LightData.
    inline std::vector<LightData*> getLightData() const;

    /// Toggle on/off verbosity.
    inline void setVerbose( const bool VERBOSE_MODE );

    /// Return true if the filename is set but the file hasn't been processed.
    inline bool isInitialized() const;

    /// Return true if the file has been processed.
    inline bool isProcessed() const;

    /// Return true if there is GeometryData.
    inline bool hasGeometry() const;

    /// Return true if there is HandleData.
    inline bool hasHandle() const;

    /// Return true if there is AnimationData.
    inline bool hasAnimation() const;

    /// Return true if there is LightData.
    inline bool hasLight() const;

    /// Return true if the FileData is verbose.
    inline bool isVerbose() const;

    /// Clear all data and unset the filename.
    /// \note this leaves the file unprocessed.
    inline void reset();

    /// Print stat info to the Debug output.
    inline void displayInfo() const;

    // TODO(Matthieu) : handle attributes in a better way than "public:"
  public:
    /// The file from which the Data are loaded.
    std::string m_filename;

    /// The time elapsed loading.
    Scalar m_loadingTime;

    /// The list of GeometryData.
    std::vector<std::unique_ptr<GeometryData>> m_geometryData;

    /// The list of HandleData.
    std::vector<std::unique_ptr<HandleData>> m_handleData;

    /// The list of AnimationData.
    std::vector<std::unique_ptr<AnimationData>> m_animationData;

    /// The list of LightData.
    std::vector<std::unique_ptr<LightData>> m_lightData;

    /// Whether the file has been processed.
    bool m_processed;

    /// Whether the FileData is verbose.
    bool m_verbose;
};

} // namespace Asset
} // namespace Ra

#include <Core/File/FileData.inl>

#endif // RADIUMENGINE_FILE_DATA_HPP
