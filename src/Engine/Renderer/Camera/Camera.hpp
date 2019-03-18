#ifndef RADIUMENGINE_CAMERA_HPP
#define RADIUMENGINE_CAMERA_HPP

#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
/**
 * The set of viewing parameters extracted from the Camera and given to the Renderer.
 */
struct RA_ENGINE_API ViewingParameters {
    Core::Matrix4 viewMatrix{Core::Matrix4::Identity()}; ///< The view matrix.
    Core::Matrix4 projMatrix{Core::Matrix4::Identity()}; ///< The projection matrix.
    Scalar dt{0};                                        ///< ???
};

/**
 * A Camera is a Component storing a camera object.
 */
class RA_ENGINE_API Camera : public Component {
  public:
    /**
     * Defines the projection type.
     */
    enum class ProjType { ORTHOGRAPHIC, PERSPECTIVE };

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    Camera( Entity* entity, const std::string& name, Scalar height, Scalar width );

    ~Camera() override;

    /**
     * Create the RenderObject for the Camera.
     */
    void initialize() override;

    /**
     * Toggle on/off displaying the Camera.
     */
    void show( bool on );

    /// \name Camera frame viewpoint
    /// \{

    /**
     * Return the Camera position.
     */
    inline Core::Vector3 getPosition() const;

    /**
     * Set the position of the camera to \p position.
     */
    inline void setPosition( const Core::Vector3& position );

    /**
     * Return the direction the Camera is looking at.
     */
    inline Core::Vector3 getDirection() const;

    /**
     * Set the direction of the Camera to \p direction.
     * The other vectors will be rotated accordingly.
     */
    inline void setDirection( const Core::Vector3& direction );

    /**
     * Return the up vector.
     */
    inline Core::Vector3 getUpVector() const;

    /**
     * Set the up vector of the Camera to \p upVector.
     * The other vectors will be rotated accordingly.
     */
    inline void setUpVector( const Core::Vector3& upVector );

    /**
     * Return the right vector.
     */
    inline Core::Vector3 getRightVector() const;

    /**
     * Return the frame of the Camera.
     * Where Y is the up vector and -Z is the direction vector.
     */
    inline Core::Transform getFrame() const;

    /**
     * Set the frame of the Camera to \p frame.
     */
    inline void setFrame( const Core::Transform& frame );

    /**
     * Apply the local transformation \p T to the Camera.
     */
    void applyTransform( const Core::Transform& T );
    /// \}

    /// \name Access to view and projection matrices.
    /// \{

    /**
     * Return the view matrix.
     */
    inline Core::Matrix4 getViewMatrix() const;

    /**
     * Return the projection matrix.
     */
    inline Core::Matrix4 getProjMatrix() const;

    /**
     * Update the projection matrix according to the current parameters.
     */
    void updateProjMatrix();
    /// \}

    /** \name Utility functions
     * \note In all screen pixels coordinates functions,
     *       Y is taken to be in standard ui-libs style coordinates,
     *       i.e.\ Y=0 is the top line and Y=height is the bottom line.
     */
    /// \{

    /**
     * Return a ray emanating from the Camera, passing by the point given by
     * screen coordinates x and y.
     */
    Core::Ray getRayFromScreen( const Core::Vector2& pix ) const;

    /**
     * Return the screen coordinates of the given point p (in world coordinates).
     */
    inline Core::Vector2 project( const Core::Vector3& p ) const;

    /**
     * Return the point on the screen plane (near plane) represented by screen coordinates pix.
     */
    inline Core::Vector3 unProject( const Core::Vector2& pix ) const;
    /// \}

    /// \name Camera parameters.
    /// \{

    /**
     * Return the projection type.
     */
    inline ProjType getType() const;

    /**
     * Set the projection type to \p projectionType.
     */
    inline void setType( const ProjType& projectionType );

    /**
     * Return the Field Of View.
     * \note Meaningless for orthogonal projection.
     */
    inline Scalar getFOV() const;

    /**
     * Set the Field Of View to \p fov.
     * \note Meaningless for orthogonal projection.
     * \warning Trigger a rebuild of the projection matrix.
     */
    inline void setFOV( Scalar fov );

    /**
     * Return the zoom factor.
     */
    inline Scalar getZoomFactor() const;

    /**
     * Set the zoom factor to \p zoomFactor.
     */
    inline void setZoomFactor( const Scalar& zoomFactor );
    /// \}

    /** \name To be deprecated
     * Currently, only the CameraInterface (i.e.\ TrackballCamera) calls these
     * methods. A rework of the rendering architecture will be done soon.
     * Thus these methods might disappear.
     */
    /// \{

    /**
     * Return the Z Near plane distance from the Camera.
     */
    inline Scalar getZNear() const;

    /**
     * Set the Z Near plane distance to 'zNear'.
     */
    inline void setZNear( Scalar zNear );

    /**
     * Return the Z Far plane distance from the Camera.
     */
    inline Scalar getZFar() const;

    /**
     * Set the Z Far plane distance to 'zFar'.
     */
    inline void setZFar( Scalar zFar );

    /**
     * Return the width of the viewport.
     */
    inline Scalar getWidth() const;

    /**
     * Return the height of the viewport.
     */
    inline Scalar getHeight() const;

    /**
     * Return the aspect ratio of the viewport.
     */
    inline Scalar getAspect() const;

    /**
     * Change the viewport size.
     */
    inline void resize( Scalar width, Scalar height );
    /// \}

  protected:
    /// Camera frame (inverse of the view matrix).
    Core::Transform m_frame{Core::Transform::Identity()};

    /// Projection matrix.
    Core::Matrix4 m_projMatrix{Core::Matrix4::Identity()};

    ProjType m_projType{ProjType ::PERSPECTIVE}; ///< Projection type.
    Scalar m_zoomFactor{1};                      ///< Zoom factor (modifies the field of view).
    Scalar m_fov{Core::Math::PiDiv4};            ///< Field of view.

    RenderObject* m_RO{nullptr}; ///< Render mesh for the Camera.

    /** \name To be deprecated
     * Currently, only the CameraInterface (i.e.\ TrackballCamera) accesses these
     * attributes. A rework of the rendering architecture will be done soon.
     * Thus these attributes might disappear.
     */
    /// \{
    Scalar m_zNear{0.1}; ///< Z Near plane distance.
    Scalar m_zFar{1000}; ///< Z Far plane distance.
    Scalar m_width{1};   ///< Viewport width (in pixels).
    Scalar m_height{1};  ///< Viewport height (in pixels).
    Scalar m_aspect{1};  ///< Aspect ratio, i.e.\ width/height. Precomputed for updateProjMatrix.
    /// \}
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Camera/Camera.inl>

#endif // RADIUMENGINE_CAMERA_HPP
