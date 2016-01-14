#ifndef RADIUMENGINE_FILE_DATA_HPP
#define RADIUMENGINE_FILE_DATA_HPP

#include <Core/CoreMacros.hpp>

#include <string>
#include <vector>
#include <memory>

namespace Ra {
namespace Asset {

class GeometryData;
class HandleData;
class AnimationData;

class FileData {
public:
    /// CONSTRUCTOR
    FileData( const std::string& filename = "",
              const bool VERBOSE_MODE = false );

    FileData( const FileData& data ) = default;

    /// DESTRUCTOR
    ~FileData();

    /// LOAD
    void loadFile( const bool FORCE_RELOAD = false );

    /// FILENAME
    inline std::string getFileName() const;

    inline void setFileName( const std::string& filename );

    /// TIMING
    inline Scalar getLoadingTime() const;

    /// DATA
    inline std::vector<  GeometryData* > getGeometryData() const;
    inline std::vector<    HandleData* > getHandleData()   const;
    inline std::vector< AnimationData* > getAnimationData() const;

    inline void setVerbose( const bool VERBOSE_MODE );

    /// QUERY
    inline bool isInitialized() const;
    inline bool isProcessed() const;
    inline bool hasGeometry() const;
    inline bool hasHandle() const;
    inline bool hasAnimation() const;
    inline bool isVerbose() const;

    /// RESET
    inline void reset();

    inline void displayInfo() const;

protected:
    /// VARIABLE
    std::string                                     m_filename;
    Scalar                                          m_loadingTime;
    std::vector< std::unique_ptr< GeometryData > >  m_geometryData;
    std::vector< std::unique_ptr< HandleData > >    m_handleData;
    std::vector< std::unique_ptr< AnimationData > > m_animationData;
    bool                                            m_processed;
    bool                                            m_verbose;
};

} // namespace Asset
} // namespace Ra

#include <Engine/Assets/FileData.inl>

#endif // RADIUMENGINE_FILE_DATA_HPP
