#include <Core/File/FileData.hpp>

namespace Ra {
    namespace Asset {
        /// CONSTRUCTOR
        FileData::FileData( const std::string& filename,
                            const bool         VERBOSE_MODE ) :
            m_filename( filename ),
            m_loadingTime( 0.0 ),
            m_geometryData(),
            m_handleData(),
            m_animationData(),
            m_lightData(),
            m_processed( false ),
            m_verbose( VERBOSE_MODE )
        {}

        /// DESTRUCTOR
        FileData::~FileData()
        {}
    } // namespace Asset
} // namespace Ra

