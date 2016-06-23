#ifndef RADIUMENGINE_CAMERAINTERFACE_HPP
#define RADIUMENGINE_CAMERAINTERFACE_HPP

#include <memory>

#include <QObject>
#include <QKeyEvent>
#include <QMouseEvent>

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Core
    {
        struct MouseEvent;
        struct KeyEvent;
    }
}

namespace Ra
{
    namespace Engine
    {
        class  Camera;
        class  Light;
    }
}

namespace Ra
{
    namespace Gui
    {

        class CameraInterface : public QObject
        {
            Q_OBJECT

        public:
            // FIXME(Charly): width / height ?
            CameraInterface( uint width, uint height );
            virtual ~CameraInterface();

            void resizeViewport( uint width, uint height );

            Core::Matrix4 getProjMatrix() const;
            Core::Matrix4 getViewMatrix() const;

            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleMousePressEvent( QMouseEvent* event ) = 0;
            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleMouseReleaseEvent( QMouseEvent* event ) = 0;
            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleMouseMoveEvent( QMouseEvent* event ) = 0;
			/// @return true if the event has been taken into account, false otherwise
			virtual bool handleWheelEvent(QWheelEvent* event) = 0;

            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleKeyPressEvent( QKeyEvent* event ) = 0;
            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleKeyReleaseEvent( QKeyEvent* event ) = 0;

            const Engine::Camera* getCamera() const { return m_camera.get();}

            void attachLight( const std::shared_ptr<Engine::Light>& light )
            {
                m_light = light;
                m_hasLightAttached = true;
            }                             

            virtual void update( Scalar dt ) = 0;

        public slots:
            void setCameraSensitivity( double sensitivity );

            void setCameraFov( double fov );
            void setCameraFovInDegrees( double fov );
            void setCameraZNear( double zNear );
            void setCameraZFar( double zFar );

            void mapCameraBehaviourToAabb( const Core::Aabb& aabb );
            void unmapCameraBehaviourToAabb();

            virtual void fitScene( const Core::Aabb& aabb ) = 0;

            virtual void setCameraPosition( const Core::Vector3& position ) = 0;
            virtual void setCameraTarget( const Core::Vector3& target ) = 0;

            virtual void resetCamera() = 0;

        signals:
            void cameraPositionChanged( const Core::Vector3& );
            void cameraTargetChanged( const Core::Vector3& );

        protected:
            Core::Aabb m_targetedAabb;

            Scalar m_targetedAabbVolume;
            Scalar m_cameraSensitivity;

            std::unique_ptr<Engine::Camera> m_camera;
            bool m_mapCameraBahaviourToAabb;

            std::shared_ptr<Engine::Light> m_light;
            bool m_hasLightAttached;
        };

    } // namespace Ra
} // namespace Engine

#endif // RADIUMENGINE_CAMERAINTERFACE_HPP
