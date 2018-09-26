#include <Core/File/CameraData.hpp>

#include "CameraData.hpp"
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Asset {

inline void CameraData::setName( const std::string& name ) {
    m_name = name;
}

inline CameraData::CameraType CameraData::getType() const {
    return m_type;
}

inline void CameraData::setType( const CameraType& type ) {
    m_type = type;
}

inline Core::Matrix4 CameraData::getFrame() const {
    return m_frame;
}

inline void CameraData::setFrame( const Core::Matrix4& frame ) {
    m_frame = frame;
}

inline Scalar CameraData::getFov() const {
    return m_fov;
}

inline void CameraData::setFov( Scalar fov ) {
    m_fov = fov;
}

inline Scalar CameraData::getZNear() const {
    return m_zNear;
}

inline void CameraData::setZNear( Scalar zNear ) {
    m_zNear = zNear;
}

inline Scalar CameraData::getZFar() const {
    return m_zFar;
}

inline void CameraData::setZFar( Scalar zFar ) {
    m_zFar = zFar;
}

inline Scalar CameraData::getZoomFactor() const {
    return m_zoomFactor;
}

inline void CameraData::setZoomFactor( Scalar zoom ) {
    m_zoomFactor = zoom;
}

inline Scalar CameraData::getAspect() const {
    return m_aspect;
}

inline void CameraData::setAspect( Scalar aspect ) {
    m_aspect = aspect;
}

inline bool CameraData::isOrthographicCamera() const {
    return ( m_type == ORTHOGRAPHIC );
}

inline bool CameraData::isPespectiveCamera() const {
    return ( m_type == PERSPECTIVE );
}

inline void CameraData::displayInfo() const {
    std::string type;
    switch ( m_type )
    {
    case ORTHOGRAPHIC:
        type = "ORTHOGRAPHIC Camera";
        break;
    case PERSPECTIVE:
        type = "PERSPECTIVE Camera";
        break;
    }
    LOG( logINFO ) << "======== Camera INFO ========";
    LOG( logINFO ) << " Name           : " << m_name;
    LOG( logINFO ) << " Type           : " << type;
}

} // namespace Asset
} // namespace Ra
