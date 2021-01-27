#pragma once
#ifndef RADIUMENGINE_GEOMETRY_SYSTEM_HPP
#    define RADIUMENGINE_GEOMETRY_SYSTEM_HPP

#    include <Engine/System/System.hpp>

namespace Ra {
namespace Engine {

class RA_ENGINE_API GeometrySystem : public System
{
  public:
    GeometrySystem();
    ~GeometrySystem() override = default;

    void handleAssetLoading( Entity* entity, const Ra::Core::Asset::FileData* fileData ) override;

    void generateTasks( Ra::Core::TaskQueue* taskQueue, const FrameInfo& frameInfo ) override;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_GEOMETRY_SYSTEM_HPP
