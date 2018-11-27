#include <Engine/System/GeometrySystem.hpp>

#include <Core/File/FileData.hpp>
#include <Core/File/GeometryData.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/Component/GeometryComponent.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace Ra {
namespace Engine {

GeometrySystem::GeometrySystem() : Ra::Engine::System() {}

void GeometrySystem::handleAssetLoading( Ra::Engine::Entity* entity,
                                         const Ra::Asset::FileData* fileData ) {
    auto geomData = fileData->getGeometryData();

    uint id = 0;

    for ( const auto& data : geomData )
    {
        std::string componentName = "FMC_" + entity->getName() + std::to_string( id++ );
        auto comp = new GeometryComponent( componentName, fileData->hasHandle(), entity );
        comp->handleMeshLoading( data );
        registerComponent( entity, comp );
    }
}

void GeometrySystem::generateTasks( Ra::Core::TaskQueue* /*taskQueue*/,
                                    const Ra::Engine::FrameInfo& frameInfo ) {
    // Do nothing, as this system only displays meshes.
}

} // namespace Engine
} // namespace Ra
