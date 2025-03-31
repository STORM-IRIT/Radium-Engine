#pragma once

#include <Engine/RaEngine.hpp>
#include <Engine/Scene/System.hpp>

namespace Ra {
namespace Core {
class TaskQueue;
namespace Asset {
class FileData;
} // namespace Asset
} // namespace Core

namespace Engine {
struct FrameInfo;

namespace Scene {
class Entity;

class RA_ENGINE_API GeometrySystem : public System
{
  public:
    GeometrySystem();
    ~GeometrySystem() override = default;

    void handleAssetLoading( Entity* entity, const Ra::Core::Asset::FileData* fileData ) override;

    void generateTasks( Ra::Core::TaskQueue* taskQueue, const FrameInfo& frameInfo ) override;
};

} // namespace Scene
} // namespace Engine
} // namespace Ra
