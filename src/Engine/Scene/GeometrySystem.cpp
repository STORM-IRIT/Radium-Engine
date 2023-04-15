#include <Engine/Scene/GeometrySystem.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/ComponentMessenger.hpp>
#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/GeometryComponent.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

GeometrySystem::GeometrySystem() : System() {}

void GeometrySystem::handleAssetLoading( Entity* entity,
                                         const Ra::Core::Asset::FileData* fileData ) {
    const auto& geomData = fileData->getGeometryData();

    uint id = 0;

    for ( const auto& data : geomData ) {
        Component* comp { nullptr };
        std::string componentName = "GEOM_" + entity->getName() + std::to_string( id++ );
        switch ( data->getType() ) {
        case Ra::Core::Asset::GeometryData::POINT_CLOUD:
            comp = new PointCloudComponent( componentName, entity, data.get() );
            break;
        case Ra::Core::Asset::GeometryData::LINE_MESH:
            //            comp = new LineMeshComponent( componentName, entity, data.get() );
            //            break;
        case Ra::Core::Asset::GeometryData::TRI_MESH:
            comp = new TriangleMeshComponent( componentName, entity, data.get() );
            break;
        case Ra::Core::Asset::GeometryData::QUAD_MESH:
            comp = new QuadMeshComponent( componentName, entity, data.get() );
            break;
        case Ra::Core::Asset::GeometryData::POLY_MESH:
            comp = new PolyMeshComponent( componentName, entity, data.get() );
            break;
        case Ra::Core::Asset::GeometryData::TETRA_MESH:
        case Ra::Core::Asset::GeometryData::HEX_MESH:
        case Ra::Core::Asset::GeometryData::UNKNOWN:
        default:
            CORE_ASSERT( false, "unsupported geometry" );
        }
        registerComponent( entity, comp );
    }

    const auto& volumeData = fileData->getVolumeData();

    id = 0;

    for ( const auto& data : volumeData ) {
        std::string componentName = "VOL_" + entity->getName() + std::to_string( id++ );
        auto comp                 = new VolumeComponent( componentName, entity, data.get() );
        registerComponent( entity, comp );
    }
}

void GeometrySystem::generateTasks( Ra::Core::TaskQueue* /*taskQueue*/,
                                    const Ra::Engine::FrameInfo& /*frameInfo*/ ) {
    // Do nothing, as this system only displays meshes.
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
