#include <Core/Asset/FileData.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// CONSTRUCTOR

void FileData::displayInfo() const {
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
    LOG( logINFO ) << "Volume loaded      : " << m_volumeData.size();
    LOG( logINFO ) << "Light loaded       : " << m_lightData.size();
    LOG( logINFO ) << "Camera loaded      : " << m_cameraData.size();
    LOG( logINFO ) << "Loading Time (sec) : " << m_loadingTime;
}
} // namespace Asset
} // namespace Core
} // namespace Ra
