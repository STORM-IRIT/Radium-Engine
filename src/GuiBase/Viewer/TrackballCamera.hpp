#ifndef RADIUMENGINE_TRACKBALLCAMERA_HPP
#define RADIUMENGINE_TRACKBALLCAMERA_HPP

#include <GuiBase/Viewer/CameraInterface.hpp>

namespace Ra
{
    namespace Gui
    {

        class TrackballCamera : public CameraInterface
        {
            Q_OBJECT

        public:
            TrackballCamera( uint width, uint height );
            virtual ~TrackballCamera();

            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleMousePressEvent( QMouseEvent* event ) override;
            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleMouseReleaseEvent( QMouseEvent* event ) override;
            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleMouseMoveEvent( QMouseEvent* event ) override;
			/// @return true if the event has been taken into account, false otherwise
			virtual bool handleWheelEvent(QWheelEvent *event) override;

            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleKeyPressEvent( QKeyEvent* event ) override;
            /// @return true if the event has been taken into account, false otherwise
            virtual bool handleKeyReleaseEvent( QKeyEvent* event ) override;

            virtual void update( Scalar dt ) override;

        public slots:
            virtual void setCameraPosition( const Core::Vector3& position ) override;
            virtual void setCameraTarget( const Core::Vector3& target ) override;
            virtual void fitScene( const Core::Aabb& aabb ) override;

            virtual void resetCamera() override;

        private:
            void handleCameraRotate( Scalar dx, Scalar dy );
            void handleCameraPan( Scalar dx, Scalar dy );
            void handleCameraZoom( Scalar dx, Scalar dy );
			void handleCameraZoom( Scalar z );

            void updatePhiTheta();

        private:
            Core::Vector3 m_trackballCenter;

            Scalar m_lastMouseX;
            Scalar m_lastMouseY;

            Scalar m_quickCameraModifier;
			Scalar m_wheelSpeedModifier;

            Scalar m_phi;
            Scalar m_theta;

            Scalar m_distFromCenter;
            Scalar m_cameraRadius;

            bool m_rotateAround;
            bool m_cameraRotateMode;
            bool m_cameraPanMode;
            // TODO(Charly): fps mode
            bool m_cameraZoomMode;

            bool m_walkingOn;
            bool m_strafingOn;
            bool m_climbingOn;

            Scalar m_walking;
            Scalar m_strafing;
            Scalar m_climbing;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_TRACKBALLCAMERA_HPP
