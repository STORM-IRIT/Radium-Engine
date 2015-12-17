#include "DummySystem.hpp"

#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include "DummyComponent.hpp"

namespace DummyPlugin
{

    DummySystem::DummySystem()
        : Ra::Engine::System()
    {
        // FIXME( Charly ): Remove this
        Ra::Core::Any::getDeclTypeFor<int>();
        Ra::Core::Any::getDeclTypeFor<Scalar>();
        Ra::Core::Any::getDeclTypeFor<bool>();
        Ra::Core::Any::getDeclTypeFor<std::string>();
        Ra::Core::Any::getDeclTypeFor<double>();
    }

    DummySystem::~DummySystem()
    {
    }

    void DummySystem::initialize()
    {
    }

    void DummySystem::handleFileLoading(Ra::Engine::Entity* entity, const std::string& filename)
    {
        LOG( logDEBUG ) << "DummySystem : loading the file " << filename << "...";

        DataVector componentsData = DummyLoader::loadFile( filename );

        if ( componentsData.empty() )
        {
            // Something wrong happened while trying to load the file
            return;
        }
        
        for (uint i = 0; i < componentsData.size(); i++)
        {
            FancyComponentData componentData = componentsData[i];
            DummyComponent* component = static_cast<DummyComponent*>(addComponentToEntity(entity));
            component->handleMeshLoading(componentData);
            
            MeshLoadingInfo loadingInfo;
            loadingInfo.filename = filename;
            loadingInfo.index = i;
            loadingInfo.vertexMap = componentData.mesh.vertexMap;
            component->setLoadingInfo(loadingInfo);
            
            callOnComponentCreationDependencies(component);   
        }
    }

    void DummySystem::handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder,
                                             const std::map<std::string, Ra::Core::Any>& data )
    {
        LOG( logDEBUG ) << "DummySystem : loading " << data.size() << " data items...";

        // Find mesh
        std::string filename;
        auto meshData = data.find( "mesh" );
        if ( meshData != data.end() )
        {
            filename = rootFolder + "/" + meshData->second.as<std::string>();
        }

        DataVector componentsData = DummyLoader::loadFile( filename );

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

        DummyComponent* component = static_cast<DummyComponent*>( addComponentToEntity( entity ) );
        component->handleMeshLoading( componentData );
        
        callOnComponentCreationDependencies(component);
    }

    Ra::Engine::Component* DummySystem::addComponentToEntityInternal( Ra::Engine::Entity* entity, uint id )
    {
        std::string componentName = "DummyComponent_" + entity->getName() + std::to_string( id );
        DummyComponent* component = new DummyComponent( componentName );

        return component;
    }

    void DummySystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        // Do nothing, as this system only displays meshes.
    }

    DummyComponent* DummySystem::addDummyToEntity( Ra::Engine::Entity* entity,
                                                               const Ra::Core::TriangleMesh& mesh )
    {
        DummyComponent* comp = static_cast<DummyComponent*>( addComponentToEntity( entity ) );
        comp->addMeshRenderObject( mesh, "Mesh RenderObject" );
        return comp;
    }

    DummyComponent* DummySystem::addDummyToEntity( Ra::Engine::Entity* entity,
                                                               const Ra::Core::TriangleMesh& mesh,
                                                               Ra::Engine::RenderTechnique* technique )
    {
        DummyComponent* comp = static_cast<DummyComponent*>( addComponentToEntity( entity ) );
        comp->addMeshRenderObject( mesh, "Mesh RenderObject", technique );
        return comp;
    }

} // namespace DummyPlugin
