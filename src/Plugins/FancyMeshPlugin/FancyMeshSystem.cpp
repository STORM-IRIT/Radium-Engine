#include <Plugins/FancyMeshPlugin/FancyMeshSystem.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Plugins/FancyMeshPlugin/FancyMeshComponent.hpp>
#include <Plugins/FancyMeshPlugin/FancyMeshLoader.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

namespace FancyMeshPlugin
{

    FancyMeshSystem::FancyMeshSystem( Ra::Engine::RadiumEngine* engine )
        : Ra::Engine::System( engine )
    {
        Ra::Core::Any::getDeclTypeFor<int        >();
        Ra::Core::Any::getDeclTypeFor<Scalar     >();
        Ra::Core::Any::getDeclTypeFor<bool       >();
        Ra::Core::Any::getDeclTypeFor<std::string>();
        Ra::Core::Any::getDeclTypeFor<double     >();
    }

    FancyMeshSystem::~FancyMeshSystem()
    {
    }

    void FancyMeshSystem::initialize()
    {
    }

    void FancyMeshSystem::handleFileLoading( const std::string& filename )
    {
        DataVector componentsData = FancyMeshLoader::loadFile( filename );

        for ( uint i = 0; i < componentsData.size(); ++i )
        {
            FancyComponentData data = componentsData[i];

            // Retrieve entity if exist, create it otherwise
            Ra::Engine::Entity* e = m_engine->getEntityManager()->getOrCreateEntity( data.name );
            e->setTransform( data.transform );

            FancyMeshComponent* component =
                static_cast<FancyMeshComponent*>( addComponentToEntity( e ) );

            component->handleMeshLoading( data );
        }
    }

    void FancyMeshSystem::handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder, 
                                             const std::map<std::string, Ra::Core::Any>& data )
    {
        LOG(logDEBUG) << "FancyMeshSystem : loading " << data.size() << " data items...";

        // Find mesh
        std::string filename;
        auto meshData = data.find("mesh");
        if ( meshData != data.end() )
        {
            filename = rootFolder + "/" + meshData->second.as<std::string>();
        }

        DataVector componentsData = FancyMeshLoader::loadFile( filename );

        if (componentsData.empty())
        {
            // Something wrong happened while trying to load the file
            return;
        }

        if (componentsData.size() > 1)
        {
            LOG(logWARNING) << "Too much objects have been loaded, some data will be ignored.";
        }

        FancyComponentData componentData = componentsData[0];

        FancyMeshComponent* component = static_cast<FancyMeshComponent*>(addComponentToEntity(entity));
        component->handleMeshLoading(componentData);
    }

    Ra::Engine::Component* FancyMeshSystem::addComponentToEntity( Ra::Engine::Entity* entity )
    {
        uint componentId = entity->getComponentsMap().size();

        std::string componentName = "FancyMeshComponent_" + entity->getName() + std::to_string( componentId++ );
        FancyMeshComponent* component = new FancyMeshComponent( componentName );
         
        component->setEntity( entity );
        component->setRenderObjectManager( m_engine->getRenderObjectManager() );

        entity->addComponent( component );
        this->addComponent( component );

        component->initialize();

        return component;
    }

    void FancyMeshSystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        // Do nothing, as this system only displays meshes.
        Ra::Core::DummyTask* task = new Ra::Core::DummyTask;
        Ra::Core::DummyTaskParams p;
        p.m_param = frameInfo.m_dt;
        task->init( &p );
        taskQueue->registerTask( task );
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
