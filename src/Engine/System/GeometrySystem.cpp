#include <Engine/System/GeometrySystem.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/Component/GeometryComponent.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {

GeometrySystem::GeometrySystem() : Ra::Engine::System() {}

void GeometrySystem::handleAssetLoading( Ra::Engine::Entity* entity,
                                         const Ra::Core::Asset::FileData* fileData ) {
    auto geomData = fileData->getGeometryData();

    uint id = 0;

    for ( const auto& data : geomData )
    {
        std::string componentName = "FMC_" + entity->getName() + std::to_string( id++ );
        auto comp = new TriangleMeshComponent( componentName, entity, data );
        registerComponent( entity, comp );
    }
}

void GeometrySystem::generateTasks( Ra::Core::TaskQueue* /*taskQueue*/,
                                    const Ra::Engine::FrameInfo& frameInfo ) {
    // Do nothing, as this system only displays meshes.
}

} // namespace Engine
} // namespace Ra
