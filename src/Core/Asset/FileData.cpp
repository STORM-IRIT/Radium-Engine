#include <Core/Asset/FileData.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// CONSTRUCTOR
FileData::FileData( const std::string& filename, const bool VERBOSE_MODE ) :
    m_filename( filename ),
    m_loadingTime( 0.0 ),
    m_geometryData(),
    m_volumeData(),
    m_handleData(),
    m_animationData(),
    m_lightData(),
    m_processed( false ),
    m_verbose( VERBOSE_MODE ) {}

/// DESTRUCTOR
FileData::~FileData() {}

} // namespace Asset
} // namespace Core
} // namespace Ra
