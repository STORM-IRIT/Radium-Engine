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
        Component* comp{nullptr};
        std::string componentName = "GEOM_" + entity->getName() + std::to_string( id++ );
        switch ( data->getType() )
        {

        case Ra::Core::Asset::GeometryData::POINT_CLOUD:
            comp = new PointCloudComponent( componentName, entity, data );
            break;
        case Ra::Core::Asset::GeometryData::LINE_MESH:
            //            comp = new LineMeshComponent( componentName, entity, data );
            //            break;
        case Ra::Core::Asset::GeometryData::TRI_MESH:
            comp = new TriangleMeshComponent( componentName, entity, data );
            break;
        case Ra::Core::Asset::GeometryData::QUAD_MESH:
        case Ra::Core::Asset::GeometryData::POLY_MESH:
        case Ra::Core::Asset::GeometryData::TETRA_MESH:
        case Ra::Core::Asset::GeometryData::HEX_MESH:
        case Ra::Core::Asset::GeometryData::UNKNOWN:
            CORE_ASSERT( false, "unsupported geometry" );
        }
        registerComponent( entity, comp );
    }

    auto volumeData = fileData->getVolumeData();

    id = 0;

    for ( const auto& data : volumeData )
    {
        std::string componentName = "VOL_" + entity->getName() + std::to_string( id++ );
        auto comp                 = new VolumeComponent( componentName, entity, data );
        registerComponent( entity, comp );
    }
}

void GeometrySystem::generateTasks( Ra::Core::TaskQueue* /*taskQueue*/,
                                    const Ra::Engine::FrameInfo& /*frameInfo*/ ) {
    // Do nothing, as this system only displays meshes.
}

} // namespace Engine
} // namespace Ra
