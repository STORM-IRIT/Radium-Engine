#ifndef RADIUMENGINE_FILE_DATA_HPP
#define RADIUMENGINE_FILE_DATA_HPP

#define DEBUG_LOAD_GEOMETRY
#define DEBUG_LOAD_HANDLE
//#define DEBUG_LOAD_ANIMATION

#include <Core/CoreMacros.hpp>

#include <string>
#include <vector>
#include <memory>

namespace Ra {
namespace Asset {

#ifdef DEBUG_LOAD_GEOMETRY
class GeometryData;
#endif

#ifdef DEBUG_LOAD_HANDLE
class HandleData;
#endif

#ifdef DEBUG_LOAD_ANIMATION
class AnimationData;
#endif

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
    inline std::vector< GeometryData* > getGeometryData() const;

#ifdef DEBUG_LOAD_HANDLE
    inline std::vector< HandleData* > getHandleData() const;
#endif

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
    inline std::vector< AnimationData* > getAnimatinData() const;
#endif

    inline void setVerbose( const bool VERBOSE_MODE );

    /// QUERY
    inline bool isInitialized() const;

    inline bool isProcessed() const;

    inline bool hasGeometry() const;

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE
    inline bool hasHandle() const;
#endif

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
    inline bool hasAnimation() const;
#endif

    inline bool isVerbose() const;

    /// RESET
    inline void reset();

    inline void displayInfo() const;

protected:
    /// VARIABLE
    std::string                                     m_filename;
    Scalar                                          m_loadingTime;
    std::vector< std::unique_ptr< GeometryData > >  m_geometryData;

#ifdef DEBUG_LOAD_HANDLE
    std::vector< std::unique_ptr< HandleData > >    m_handleData;
#endif

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
    std::vector< std::unique_ptr< AnimationData > > m_animationData;
#endif
    bool                                            m_processed;
    bool                                            m_verbose;
};

} // namespace Asset
} // namespace Ra

#include <Engine/Assets/FileData.inl>

#endif // RADIUMENGINE_FILE_DATA_HPP
