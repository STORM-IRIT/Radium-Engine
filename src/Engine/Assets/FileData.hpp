#ifndef RADIUMENGINE_FILE_DATA_HPP
#define RADIUMENGINE_FILE_DATA_HPP

#define DEBUG_LOAD_GEOMETRY
//#define DEBUG_LOAD_HANDLE
//#define DEBUG_LOAD_ANIMATION


#include <string>
#include <memory>

#include <assimp/scene.h>

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

class FileData {
public:
    /// CONSTRUCTOR
    FileData( const std::string& filename = "",
              const bool VERBOSE_MODE = false ) :
        m_filename( filename ),
        m_loadingTime( 0.0 ),

        // FIXME(Charly): Needs to be fixed to be compiled
    #ifdef DEBUG_LOAD_HANDLE
        m_handleData( nullptr ),
    #endif

        // FIXME(Charly): Needs to be fixed to be compiled
    #ifdef DEBUG_LOAD_ANIMATION
        m_animationData( nullptr ),
    #endif
        m_processed( false ),
        m_verbose( VERBOSE_MODE ) {
        loadFile();
    }

    FileData( const FileData& data ) = default;

    /// DESTRUCTOR
    ~FileData() { }

    /// LOAD
    void loadFile( const bool FORCE_RELOAD = false );

    /// FILENAME
    inline std::string getFileName() const {
        return m_filename;
    }

    inline void setFileName( const std::string& filename ) {
        m_filename = filename;
    }

    /// TIMING
    inline Scalar getLoadingTime() const {
        return m_loadingTime;
    }

    /// DATA
    inline std::vector< GeometryData* > getGeometryData() const {
        std::vector< GeometryData* > list;
        list.reserve( m_geometryData.size() );
        for( const auto& item : m_geometryData ) {
            list.push_back( item.get() );
        }
        return list;
    }

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE
    inline std::vector< HandleData* > getHandleData() const {
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
    inline std::vector< AnimationData* > getAnimatinData() const {
        std::vector< AnimationData* > list;
        list.reserve( m_animationData.size() );
        for( const auto& item : m_animationData ) {
            list.push_back( item.get() );
        }
        return list;
    }
#endif

    inline void setVerbose( const bool VERBOSE_MODE ) {
        m_verbose = VERBOSE_MODE;
    }

    /// QUERY
    inline bool isInitialized() const {
        return ( ( m_filename != "" ) && !m_processed );
    }

    inline bool isProcessed() const {
        return m_processed;
    }

    inline bool hasGeometry() const {
        return ( !m_geometryData.empty() );
    }

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE
    inline bool hasHandle() const {
        return ( !m_handleData.empty() );
    }
#endif

    // FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION
    inline bool hasAnimation() const {
        return ( !m_animationData.empty() );
    }
#endif

    inline bool isVerbose() const {
        return m_verbose;
    }

    /// RESET
    inline void reset()
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

    void displayInfo() const;

protected:
    /// VARIABLE
    std::string                                     m_filename;
    Scalar                                          m_loadingTime;
    std::vector< std::unique_ptr< GeometryData > >  m_geometryData;

    // FIXME(Charly): Needs to be fixed to be compiled
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

#endif // RADIUMENGINE_FILE_DATA_HPP
