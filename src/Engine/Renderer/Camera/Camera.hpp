#ifndef RADIUMENGINE_CAMERA_HPP
#define RADIUMENGINE_CAMERA_HPP

#include <mutex>
#include <thread>

#include <Core/Math/Vector.hpp>
#include <Core/Math/Matrix.hpp>

namespace Ra { namespace Engine {

// The class Camera defines a camera in 3D space with
class Camera
{
public:
    /// -------------------- ///
    /// ENUM
    /// -------------------- ///
    // Define if the transformation applied to the camera
    // is constrained to the FOCAL POINT or not.
    enum class ModeType { TARGET, FREE };
    // Define if the projection type.
    enum class ProjType { ORTHOGRAPHIC, PERSPECTIVE };




    /// -------------------- ///
    /// CONSTRUCTOR
    /// -------------------- ///
    // Default constructor
    Camera();

    // Copy constructor
    Camera( const Camera& cam );




    /// -------------------- ///
    /// DESTRUCTOR
    /// -------------------- ///
    // Default destructor
    ~Camera();




    /// -------------------- ///
    /// FRAME
    /// -------------------- ///
    // Return the frame of the camera
    inline Core::Transform getFrame() const;

    // Set the frame of the camera to 'frame'
    inline void setFrame( const Core::Transform& frame );

    // Return the position.
    inline Core::Vector3 getPosition() const;

    // Set the position of the camera to 'position'.
    // If 'mode' is TARGET, the camera will be rotated in order to keep facing the FOCAL POINT
    inline void setPosition( const Core::Vector3& position,
                             const ModeType mode = ModeType::FREE );

    // Return the direction.
    inline Core::Vector3 getDirection() const;

    // Set the direction of the camera to 'direction'.
    // The other vectors will be rotated accordingly.
    inline void setDirection( const Core::Vector3& direction );

    // Return the up vector.
    inline Core::Vector3 getUpVector() const;

    // Set the up vector of the camera to 'upVector'.
    // The other vectors will be rotated accordingly.
    inline void setUpVector( const Core::Vector3& upVector );

    // Return the right vector.
    inline Core::Vector3 getRightVector() const;

    // Set the right vector of the camera to 'rightVector'.
    // The other vectors will be rotated accordingly.
    inline void setRightVector( const Core::Vector3& rightVector );

    // Apply the transformation 'T' to the camera.
    // If 'mode' is TARGET, the camera will be rotated in order to keep facing the FOCAL POINT
    void applyTransform( const Core::Transform& T,
                         const ModeType mode = ModeType::FREE );




    /// -------------------- ///
    /// FIELD OF VIEW
    /// -------------------- ///
    // Return the Field Of View.
    inline Scalar getFOV() const;

    // Set the Field Of View to 'fov'.
    inline void setFOV( const Scalar fov );




    /// -------------------- ///
    /// Z NEAR
    /// -------------------- ///
    // Return the Z Near plane distance from the camera.
    inline Scalar getZNear() const;

    // Set the Z Near plane distance to 'zNear'.
    inline void setZNear( const Scalar zNear );




    /// -------------------- ///
    /// Z FAR
    /// -------------------- ///
    // Return the Z Far plane distance from the camera.
    inline Scalar getZFar() const;

    // Set the Z Far plane distance to 'zFar'.
    inline void setZFar( const Scalar zFar );




    /// -------------------- ///
    /// FOCAL POINT
    /// -------------------- ///
    // Return the focal point.
    inline Core::Vector3 getTargetPoint() const;

    // Set the focal point to 'focalPoint'.
    inline void setTargetPoint( const Core::Vector3& targetPoint );

    // Return the distance of the  focal point from the camera.
    inline Scalar getFocalPointDistance() const;

    // Set the focal point distance from the camera to 'focalPointDistance'.
    inline void setFocalPointDistance( const Scalar focalPointDistance );



    /// -------------------- ///
    /// ZOOM FACTOR
    /// -------------------- ///
    // Return the zoom factor.
    inline Scalar getZoomFactor() const;

    // Set the zoom factor to 'zoomFactor'.
    inline void setZoomFactor( const Scalar& zoomFactor );




    /// -------------------- ///
    /// PROJECTION TYPE
    /// -------------------- ///
    // Return the projection type.
    inline ProjType getProjType() const;

    // Set the projection type to 'projectionType'.
    inline void setProjType( const ProjType& projectionType );




    /// -------------------- ///
    /// VIEW MATRIX
    /// -------------------- ///
    // Return the view matrix.
    inline Core::Matrix4 getViewMatrix() const;

    // Set the view matrix to 'viewMatrix'.
    inline void setViewMatrix( const Core::Matrix4& viewMatrix );

    // Update the view matrix.
    void updateViewMatrix();




    /// -------------------- ///
    /// PROJECTION MATRIX
    /// -------------------- ///
    // Return the projection matrix.
    inline Core::Matrix4 getProjMatrix() const;

    // Set the projection matrix to 'projectionMatrix'.
    inline void setProjMatrix( const Core::Matrix4& projectionMatrix );

    // Update the projection matrix accordingly to the 'width' and 'height' of the viewport.
    void updateProjMatrix( const Scalar& width, const Scalar& height );



    /// ------------------------ ///
    /// HIGH LEVEL MANIPULATIONS
    /// ------------------------ ///
    // Move the camera to the right by given amount
    inline void strafeRight(Scalar amount);
    // Move the camera to the left by given amount
    inline void strafeLeft(Scalar amount);
    // Move the camera forward by given amount
    inline void walkForward(Scalar amount);
    // Move the camera backward by given amount
    inline void walkBackward(Scalar amount);

    // Rotate the camera upside by given amount
    inline void rotateUp(Scalar amount);
    // Rotate the camera downside by given amount
    inline void rotateDown(Scalar amount);
    // Rotate the camera to the left by given amount
    inline void rotateLeft(Scalar amount);
    // Rotate the camera to the right by given amount
    inline void rotateRight(Scalar amount);

private:
    /// -------------------- ///
    /// FRAME
    /// -------------------- ///
    // Compute the rotation that brings 'v0' in 'v1'.
    // If 'v0' and 'v1' are opposite, uses 'defaultAxis' as the default rotation axis
    inline Core::Transform computeRotation( const Core::Vector3& v0,
                                      const Core::Vector3& v1,
                                      const Core::Vector3& defaultAxis ) const;

protected:
    /// -------------------- ///
    /// VARIABLE
    /// -------------------- ///
    Core::Transform m_frame;      // Camera frame

    Scalar    m_fov;        // Field of view
    Scalar    m_zNear;      // Z Near plane distance
    Scalar    m_zFar;       // Z Far plane distance

    Scalar    m_focalPoint; // Focal point distance

    Scalar    m_zoomFactor; // Zoom factor

    ProjType  m_projType;   // Projection type

    Core::Matrix4   m_viewMatrix; // View matrix
    Core::Matrix4   m_projMatrix; // Projection matrix

    std::mutex m_cameraMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Camera/Camera.inl>

#endif // RADIUMENGINE_CAMERA_HPP
