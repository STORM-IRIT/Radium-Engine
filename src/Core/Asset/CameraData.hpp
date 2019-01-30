#ifndef RADIUMENGINE_CAMERA_DATA_HPP
#define RADIUMENGINE_CAMERA_DATA_HPP

#include <memory>
#include <string>
#include <vector>

#include <Core/Asset/AssetData.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// The CameraData class stores the data related to one of the Cameras of a loaded scene.
/// \warning Stored data may change soon w.r.t. Engine::Camera.
class RA_CORE_API CameraData : public AssetData {

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// The projection type of the Camera.
    enum CameraType { ORTHOGRAPHIC, PERSPECTIVE };

    CameraData( const std::string& name = "", const CameraType& projType = PERSPECTIVE );

    CameraData( const CameraData& data ) = default;

    ~CameraData();

    /// Return the name of the Camera.
    inline void setName( const std::string& name );

    /// Return the projection type of the Camera.
    inline CameraType getType() const;

    /// Set the projection type of the Camera.
    inline void setType( const CameraType& type );

    /// Return the transformation of the Camera.
    inline const Core::Matrix4& getFrame() const;

    /// Set the transformation of the Camera.
    inline void setFrame( const Core::Matrix4& frame );

    /// Return the Field of View of the Camera.
    inline Scalar getFov() const;

    /// Set the Field of View of the Camera.
    inline void setFov( Scalar fov );

    /// Return the distance from the near plane to the Camera.
    inline Scalar getZNear() const;

    /// Set the distance from the near plane to the Camera.
    inline void setZNear( Scalar zNear );

    /// Return the distance from the far plane to the Camera.
    inline Scalar getZFar() const;

    /// Set the distance from the far plane to the Camera.
    inline void setZFar( Scalar zFar );

    /// Return the zoom factor of the Camera.
    inline Scalar getZoomFactor() const;

    /// Set the zoom factor of the Camera.
    inline void setZoomFactor( Scalar zoom );

    /// Return the aspect ratio of the Camera.
    inline Scalar getAspect() const;

    /// Set the aspect ratio of the Camera.
    inline void setAspect( Scalar aspect );

    /// Return true if the Camera is an Orthographic one.
    inline bool isOrthographicCamera() const;

    /// Return true if the Camera is a Perspective one.
    inline bool isPespectiveCamera() const;

    /// Print stat info to the Debug output.
    inline void displayInfo() const;

  protected:
    /// The transformation of the Camera.
    Core::Matrix4 m_frame;

    /// The type of Camera.
    CameraType m_type;

    /// The field of view of the Camera.
    Scalar m_fov;

    /// The distance from the near plane to the Camera.
    Scalar m_zNear;

    /// The distance from the far plane to the Camera.
    Scalar m_zFar;

    /// The zoom factor of the Camera.
    Scalar m_zoomFactor;

    /// The aspect ratio of the Camera.
    Scalar m_aspect;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/CameraData.inl>

#endif // RADIUMENGINE_CAMERA_DATA_HPP
