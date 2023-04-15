#include <Core/Asset/HandleData.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

void HandleData::displayInfo() const {
    using namespace Core::Utils; // log
    std::string type;
    switch ( m_type ) {
    case POINT_CLOUD:
        type = "POINT CLOUD";
        break;
    case SKELETON:
        type = "SKELETON";
        break;
    case CAGE:
        type = "CAGE";
        break;
    case UNKNOWN:
    default:
        type = "UNKNOWN";
        break;
    }
    LOG( logINFO ) << "======== HANDLE INFO ========";
    LOG( logINFO ) << " Name            : " << getName();
    LOG( logINFO ) << " Type            : " << type;
    LOG( logINFO ) << " Element #       : " << m_component.size();
    LOG( logINFO ) << " Edge #          : " << m_edge.size();
    LOG( logINFO ) << " Face #          : " << m_face.size();
    LOG( logINFO ) << " Need EndNodes ? : " << ( ( m_endNode ) ? "YES" : "NO" );
}

} // namespace Asset
} // namespace Core
} // namespace Ra
