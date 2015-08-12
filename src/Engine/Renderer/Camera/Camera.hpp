#ifndef RADIUMENGINE_CAMERA_HPP
#define RADIUMENGINE_CAMERA_HPP

#include <mutex>
#include <thread>

#include <Core/Math/Math.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {

        // The class Camera defines a camera in 3D space with
        class RA_API Camera
        {
        public:
            // Define if the projection type.
            enum class ProjType
            {
                ORTHOGRAPHIC, PERSPECTIVE
            };

            ///
            /// Constructor & Destructor
            ///
            RA_CORE_ALIGNED_NEW

            // Default constructor with usual default values.
            Camera( Scalar height, Scalar width );

            // Copy constructor
            Camera( const Camera& cam ) = default;

            /// DESTRUCTOR
            ~Camera();

            ///
            /// Getters and setters for view matrix parameters.
            ///
            // Return the frame of the camera.
            // Where Y is the up vector and -Z is the direction vector.
            inline Core::Transform getFrame() const;

            // Set the frame of the camera to 'frame'
            inline void setFrame( const Core::Transform& frame );

            // Return the position.
            inline Core::Vector3 getPosition() const;

            // Set the position of the camera to 'position'.
            inline void setPosition( const Core::Vector3& position );

            // Return the direction the camera is looking at.
            inline Core::Vector3 getDirection() const;

            // Set the direction of the camera to 'direction'.
            // The other vectors will be rotated accordingly.
            inline void setDirection( const Core::Vector3& direction );

            // Return the up vector.
            inline Core::Vector3 getUpVector() const;

            // Set the up vector of the camera to 'upVector'.
            // The other vectors will be rotated accordingly.
            inline void setUpVector( const Core::Vector3& upVector );

            inline Core::Vector3 getRightVector() const;


            // Apply the transformation 'T' to the camera.
            void applyTransform( const Core::Transform& T );


            //
            // Getters and setters for projection matrix parameters.
            // Will trigger a rebuild of the projection matrix
            //

            // Return the Field Of View.
            inline Scalar getFOV() const;

            // Set the Field Of View to 'fov'.
            inline void setFOV( const Scalar fov );

            // Return the Z Near plane distance from the camera.
            inline Scalar getZNear() const;

            // Set the Z Near plane distance to 'zNear'.
            inline void setZNear( const Scalar zNear );

            // Return the Z Far plane distance from the camera.
            inline Scalar getZFar() const;

            // Set the Z Far plane distance to 'zFar'.
            inline void setZFar( const Scalar zFar );

            // Return the zoom factor.
            inline Scalar getZoomFactor() const;

            // Set the zoom factor to 'zoomFactor'.
            inline void setZoomFactor( const Scalar& zoomFactor );

            // Return the projection type.
            inline ProjType getProjType() const;

            // Set the projection type to 'projectionType'.
            inline void setProjType( const ProjType& projectionType );

            // Return the dimensions of the viewport.
            inline Scalar getWidth() const;
            inline Scalar getHeight() const;

            // Change the viewport size.
            inline void resize( Scalar width, Scalar height );

            /// -------------------- ///
            /// VIEW MATRIX
            /// -------------------- ///
            // Return the view matrix.
            inline Core::Matrix4 getViewMatrix() const;


            /// -------------------- ///
            /// PROJECTION MATRIX
            /// -------------------- ///
            // Return the projection matrix.
            inline Core::Matrix4 getProjMatrix() const;

            // Update the projection matrix according to the current parameters.
            void updateProjMatrix();


        protected:
            /// -------------------- ///
            /// VARIABLE
            /// -------------------- ///
            Core::Transform m_frame;      // Camera frame (inverse of the view matrix)
            Core::Matrix4   m_projMatrix; // Projection matrix

            Scalar    m_fov;        // Field of view
            Scalar    m_zNear;      // Z Near plane distance
            Scalar    m_zFar;       // Z Far plane distance

            Scalar    m_zoomFactor; // Zoom factor (modifies the field of view)

            Scalar m_width;
            Scalar m_height;

            ProjType  m_projType;   // Projection type
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Camera/Camera.inl>

#endif // RADIUMENGINE_CAMERA_HPP
