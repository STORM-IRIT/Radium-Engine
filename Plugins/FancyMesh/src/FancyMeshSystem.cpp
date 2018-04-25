#include <FancyMeshSystem.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Utils/StringUtils.hpp>
#include <Core/Utils/Task.hpp>
#include <Core/Utils/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <FancyMeshComponent.hpp>

namespace FancyMeshPlugin {

FancyMeshSystem::FancyMeshSystem() : Ra::Engine::System() {}

FancyMeshSystem::~FancyMeshSystem() {}

void FancyMeshSystem::handleAssetLoading( Ra::Engine::Entity* entity,
                                          const Ra::Core::Asset::FileData* fileData ) {
    auto geomData = fileData->getGeometryData();

    uint id = 0;

    for ( const auto& data : geomData )
    {
        std::string componentName = "FMC_" + entity->getName() + std::to_string( id++ );
        FancyMeshComponent* comp =
            new FancyMeshComponent( componentName, fileData->hasHandle(), entity );
        comp->handleMeshLoading( data );
        registerComponent( entity, comp );
    }
}

void FancyMeshSystem::generateTasks( Ra::Core::Utils::TaskQueue* taskQueue,
                                     const Ra::Engine::FrameInfo& frameInfo ) {
    // Do nothing, as this system only displays meshes.
}

} // namespace FancyMeshPlugin
