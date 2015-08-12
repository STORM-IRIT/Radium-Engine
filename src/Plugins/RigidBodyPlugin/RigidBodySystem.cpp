#include <Plugins/RigidBodyPlugin/RigidBodySystem.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Entity/FrameInfo.hpp>

#include <Plugins/RigidBodyPlugin/RigidBodyComponent.hpp>
#include <Plugins/RigidBodyPlugin/RigidBodyTask.hpp>

namespace RigidBodyPlugin
{

    RigidBodySystem::RigidBodySystem( Ra::Engine::RadiumEngine* engine )
        : Ra::Engine::System( engine )
    {
    }

    RigidBodySystem::~RigidBodySystem()
    {
    }

    void RigidBodySystem::initialize()
    {
    }

    void RigidBodySystem::handleFileLoading( const std::string& filename )
    {
    }

    Ra::Engine::Component* RigidBodySystem::addComponentToEntity( Ra::Engine::Entity* entity )
    {
        uint componentId = entity->getComponents().size();

        std::string componentName = "RigidBodyComponent_" + entity->getName() + std::to_string( componentId++ );
        RigidBodyComponent* comp = new RigidBodyComponent( componentName );

        comp->setEntity( entity );
        comp->setRenderObjectManager( m_engine->getRenderObjectManager() );

        entity->addComponent( comp );
        this->addComponent( comp );

        comp->initialize();

        return comp;
    }

    void RigidBodySystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        RigidBodyTask* task = new RigidBodyTask;
        RigidBodyTaskParams p;

        // FIXME(Charly): Cache this
        std::vector<RigidBodyComponent*> components;
        for ( const auto c : m_components )
        {
            components.push_back( static_cast<RigidBodyComponent*>( c.second.get() ) );
        }

        p.rigidBodies = components;
        p.dt = frameInfo.m_dt;

        task->init( &p );
        taskQueue->registerTask( task );
    }

    RigidBodyComponent* RigidBodySystem::addRigidBodyToEntity( Ra::Engine::Entity* entity, Scalar mass )
    {
        RigidBodyComponent* comp = static_cast<RigidBodyComponent*>( addComponentToEntity( entity ) );
        comp->setMass( mass );
        return comp;
    }

} // namespace RigidBodyPlugin
