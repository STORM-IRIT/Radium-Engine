#ifndef RADIUMENGINE_VIEWINGPARAMETERS_HPP
#define RADIUMENGINE_VIEWINGPARAMETERS_HPP

#include <Core/Types.hpp>

namespace Ra {
namespace Engine {
/// the set of viewing parameters extracted from the camera and given to the renderer
struct RA_ENGINE_API ViewingParameters {
    Core::Matrix4 viewMatrix{Core::Matrix4::Identity()};
    Core::Matrix4 projMatrix{Core::Matrix4::Identity()};
    Scalar dt{0};
};
} // namespace Engine
} // namespace Ra
#endif // RADIUMENGINE_VIEWINGPARAMETERS_HPP
