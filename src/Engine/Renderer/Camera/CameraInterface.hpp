#ifndef RADIUMENGINE_CAMERAINTERFACE_HPP
#define RADIUMENGINE_CAMERAINTERFACE_HPP

#include <memory>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Vector.hpp>
#include <Core/Math/Matrix.hpp>

namespace Ra { namespace Core   { struct MouseEvent; } }
namespace Ra { namespace Core   { struct KeyEvent; } }
namespace Ra { namespace Engine { struct Camera; } }

namespace Ra { namespace Engine {

class CameraInterface
{
public:
	// FIXME(Charly): width / height ?
	CameraInterface(uint width, uint height);
	virtual ~CameraInterface();

	void resizeViewport(uint width, uint height);

	Core::Matrix4 getProjMatrix() const;
	Core::Matrix4 getViewMatrix() const;

	void setCameraSensitivity(Scalar sensitivity);

	void setCameraFov(Scalar fov);
	void setCameraFovInDegrees(Scalar fov);
	void setCameraZNear(Scalar zNear);
	void setCameraZFar(Scalar zFar);

	void mapCameraBehaviourToAabb(const Core::Aabb& aabb);
	void unmapCameraBehaviourToAabb();

	virtual void moveCameraToFitAabb(const Core::Aabb& aabb);
	virtual void moveCameraTo(const Core::Vector3& position);
	
	/// @return true if the event has been taken into account, false otherwise
	virtual bool handleMouseEvent(Core::MouseEvent* event) = 0;
	/// @return true if the event has been taken into account, false otherwise
	virtual bool handleKeyEvent(Core::KeyEvent* event) = 0;

protected:
	uint m_width;
	uint m_height;

	mutable bool m_viewIsDirty;
	mutable bool m_projIsDirty;

	mutable std::unique_ptr<Camera> m_camera;

	bool m_mapCameraBahaviourToAabb;
	Core::Aabb m_targetedAabb;
	Scalar m_targetedAabbVolume;

	Scalar m_cameraSensitivity;
};

} // namespace Ra
} // namespace Engine

#endif // RADIUMENGINE_CAMERAINTERFACE_HPP