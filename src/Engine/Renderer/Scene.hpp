#ifndef RADIUMENGINE_SCENE_HPP
#define RADIUMENGINE_SCENE_HPP

#include <Core/CoreMacros.hpp>

#include <vector>
#include <memory>
#include <array>

#include <Core/Utils/Singleton.hpp>

#include <Engine/RaEngine.hpp>

namespace Ra
{
namespace Engine
{

class Camera;
class Light;
class RenderObject;

// NOTE(Charly): Could be interesting to have kind of a scene graph
class RA_ENGINE_API Scene
{
    // FIXME(Charly): Singleton ?
    RA_SINGLETON_INTERFACE( Scene );
public:
    enum
    {
        SPECIAL_RO_DEBUG = 0,
        SPECIAL_RO_UI,
        SPECIAL_RO_XRAY,
        SPECIAL_RO_COUNT
    };

    using CameraPtr             = std::shared_ptr<Camera>;
    using LightPtr              = std::shared_ptr<Light>;
    using RenderObjectPtr       = std::shared_ptr<RenderObject>;

    using CameraPtrVec          = std::vector<CameraPtr>;
    using LightPtrVec           = std::vector<LightPtr>;
    using RenderObjectPtrVec    = std::vector<RenderObjectPtr>;

public:
    // FIXME(Charly): Is this the way this should be done ?
    void update();

public:
    CameraPtrVec        cameras;
    LightPtrVec         lights;
    RenderObjectPtrVec  renderObjects;

    std::array<RenderObjectPtrVec, SPECIAL_RO_COUNT> specialRenderObjects;
};
}
}

#endif // RADIUMENGINE_SCENE_HPP
