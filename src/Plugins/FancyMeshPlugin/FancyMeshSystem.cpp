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

    Ra::Engine::Component* FancyMeshSystem::addComponentToEntity( Ra::Engine::Entity* entity )
    {
        uint componentId = entity->getComponents().size();

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
#if 0
        Ra::Core::DummyTask* task = new Ra::Core::DummyTask;
        Ra::Core::DummyTaskParams p;
        p.m_param = frameInfo.m_dt;
        task->init( &p );
        taskQueue->queueTask( taskQueue->registerTask( task ) );
#endif
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
