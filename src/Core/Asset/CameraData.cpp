#include <Core/Asset/CameraData.hpp>

#include <Core/Math/Math.hpp>

namespace Ra {
namespace Core {
namespace Asset {

CameraData::CameraData( const std::string& name, const CameraType& type ) :
    AssetData( name ),
    m_frame( Core::Matrix4::Identity() ),
    m_type( type ),
    m_fov( Core::Math::PiDiv4 ),
    m_zNear( Scalar( 0.1f ) ),
    m_zFar( Scalar( 1000.0f ) ),
    m_zoomFactor( Scalar( 1.0f ) ),
    m_aspect( 1.0f ) {}

CameraData::~CameraData() {}

} // namespace Asset
} // namespace Core
} // namespace Ra
