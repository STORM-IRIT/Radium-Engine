#ifndef RADIUMENGINE_CAMERA_HPP
#define RADIUMENGINE_CAMERA_HPP

#include <Core/Math/Matrix.hpp>

namespace Ra
{

class Camera
{
public:
    Matrix4 getViewMatrix() const { return Matrix4::Identity(); }
    Matrix4 getProjMatrix() const { return Matrix4::Identity(); }
};

} // namespace Ra

#endif // RADIUMENGINE_CAMERA_HPP
