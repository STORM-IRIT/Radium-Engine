#ifndef RADIUMENGINE_GEOMETRY_SYSTEM_HPP
#define RADIUMENGINE_GEOMETRY_SYSTEM_HPP

#include <Engine/System/System.hpp>

namespace Ra {
namespace Core {
struct TriangleMesh;
}
} // namespace Ra

namespace Ra {
namespace Engine {
class Entity;
struct RenderTechnique;
class Component;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

class RA_ENGINE_API GeometrySystem : public System {
  public:
    GeometrySystem();
    virtual ~GeometrySystem();

    void handleAssetLoading( Entity* entity, const Ra::Asset::FileData* fileData ) override;

    void generateTasks( Ra::Core::TaskQueue* taskQueue, const FrameInfo& frameInfo ) override;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_SYSTEM_HPP
