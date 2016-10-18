#include <GuiBase/Viewer/CameraInterface.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra
{

    Gui::CameraInterface::CameraInterface( uint width, uint height )
        : m_cameraSensitivity( 1.0 )
        , m_hasLightAttached( false )
    {
        m_camera.reset( new Engine::Camera( Scalar( height ), Scalar( width ) ) );

        setCameraFovInDegrees( 60.0 );
        setCameraZNear( 0.1 );
        setCameraZFar( 1000.0 );
    }

    Gui::CameraInterface::~CameraInterface()
    {
    }

    void Gui::CameraInterface::resizeViewport( uint width, uint height )
    {
        m_camera->resize( Scalar( width ), Scalar( height ) );
    }

    Core::Matrix4 Gui::CameraInterface::getProjMatrix() const
    {
        return m_camera->getProjMatrix();
    }

    Core::Matrix4 Gui::CameraInterface::getViewMatrix() const
    {
        return m_camera->getViewMatrix();
    }

    void Gui::CameraInterface::setCameraSensitivity( double sensitivity )
    {
        m_cameraSensitivity = sensitivity;
    }

    void Gui::CameraInterface::setCameraFov( double fov )
    {
        m_camera->setFOV( fov );
    }

    void Gui::CameraInterface::setCameraFovInDegrees( double fov )
    {
        m_camera->setFOV( fov * Core::Math::toRad );
    }

    void Gui::CameraInterface::setCameraZNear( double zNear )
    {
        m_camera->setZNear( zNear );
    }

    void Gui::CameraInterface::setCameraZFar( double zFar )
    {
        m_camera->setZFar( zFar );
    }

    void Gui::CameraInterface::mapCameraBehaviourToAabb( const Core::Aabb& aabb )
    {
        m_targetedAabb = aabb;
        m_targetedAabbVolume = aabb.volume();
        m_mapCameraBahaviourToAabb = true;
    }

    void Gui::CameraInterface::unmapCameraBehaviourToAabb()
    {
        m_mapCameraBahaviourToAabb = false;
    }
}

