#include <Plugins/FancyMesh/FancyMeshSystem.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Plugins/FancyMesh/FancyMeshComponent.hpp>
#include <Plugins/FancyMesh/FancyMeshLoader.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace FancyMeshPlugin
{

    FancyMeshSystem::FancyMeshSystem()
        : Ra::Engine::System()
    {
        Ra::Core::Any::getDeclTypeFor<int>();
        Ra::Core::Any::getDeclTypeFor<Scalar>();
        Ra::Core::Any::getDeclTypeFor<bool>();
        Ra::Core::Any::getDeclTypeFor<std::string>();
        Ra::Core::Any::getDeclTypeFor<double>();
    }

    FancyMeshSystem::~FancyMeshSystem()
    {
    }

    void FancyMeshSystem::initialize()
    {
    }

    void FancyMeshSystem::handleFileLoading(Ra::Engine::Entity* entity, const std::string& filename)
    {
        LOG( logDEBUG ) << "FancyMeshSystem : loading the file " << filename << "...";

        DataVector componentsData = FancyMeshLoader::loadFile( filename );

        if ( componentsData.empty() )
        {
            // Something wrong happened while trying to load the file
            return;
        }
        
        for (uint i = 0; i < componentsData.size(); i++)
        {
            FancyComponentData componentData = componentsData[i];
            FancyMeshComponent* component = static_cast<FancyMeshComponent*>(addComponentToEntity(entity));
            component->handleMeshLoading(componentData);
            
            MeshLoadingInfo loadingInfo;
            loadingInfo.filename = filename;
            loadingInfo.index = i;
            loadingInfo.vertexMap = componentData.mesh.vertexMap;
            component->setLoadingInfo(loadingInfo);
            
            callOnComponentCreationDependencies(component);   
        }
    }

    void FancyMeshSystem::handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder,
                                             const std::map<std::string, Ra::Core::Any>& data )
    {
        LOG( logDEBUG ) << "FancyMeshSystem : loading " << data.size() << " data items...";

        // Find mesh
        std::string filename;
        auto meshData = data.find( "mesh" );
        if ( meshData != data.end() )
        {
            filename = rootFolder + "/" + meshData->second.as<std::string>();
        }

        DataVector componentsData = FancyMeshLoader::loadFile( filename );

        if ( componentsData.empty() )
        {
            // Something wrong happened while trying to load the file
            return;
        }

        if ( componentsData.size() > 1 )
        {
            LOG( logWARNING ) << "Too much objects have been loaded, some data will be ignored.";
        }

        FancyComponentData componentData = componentsData[0];

        FancyMeshComponent* component = static_cast<FancyMeshComponent*>( addComponentToEntity( entity ) );
        component->handleMeshLoading( componentData );
        
        callOnComponentCreationDependencies(component);
    }

    Ra::Engine::Component* FancyMeshSystem::addComponentToEntityInternal( Ra::Engine::Entity* entity, uint id )
    {
        std::string componentName = "FancyMeshComponent_" + entity->getName() + std::to_string( id );
        FancyMeshComponent* component = new FancyMeshComponent( componentName );

        return component;
    }

    void FancyMeshSystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        // Do nothing, as this system only displays meshes.
    }

    FancyMeshComponent* FancyMeshSystem::addFancyMeshToEntity( Ra::Engine::Entity* entity,
                                                               const Ra::Core::TriangleMesh& mesh )
    {
        FancyMeshComponent* comp = static_cast<FancyMeshComponent*>( addComponentToEntity( entity ) );
        comp->addMeshRenderObject( mesh, "Mesh RenderObject" );
        return comp;
    }

    FancyMeshComponent* FancyMeshSystem::addFancyMeshToEntity( Ra::Engine::Entity* entity,
                                                               const Ra::Core::TriangleMesh& mesh,
                                                               Ra::Engine::RenderTechnique* technique )
    {
        FancyMeshComponent* comp = static_cast<FancyMeshComponent*>( addComponentToEntity( entity ) );
        comp->addMeshRenderObject( mesh, "Mesh RenderObject", technique );
        return comp;
    }

} // namespace FancyMeshPlugin
