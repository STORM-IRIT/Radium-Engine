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
        Ra::Core::Any::getDeclTypeFor<int        >();
        Ra::Core::Any::getDeclTypeFor<Scalar     >();
        Ra::Core::Any::getDeclTypeFor<bool       >();
        Ra::Core::Any::getDeclTypeFor<std::string>();
        Ra::Core::Any::getDeclTypeFor<double     >();
    }

    RigidBodySystem::~RigidBodySystem()
    {
    }

    void RigidBodySystem::initialize()
    {
    }

    void RigidBodySystem::handleDataLoading(Ra::Engine::Entity* entity, const std::string& rootFolder, 
                                            const std::map<std::string, Ra::Core::Any>& data)
    {
        LOG(logDEBUG) << "RigidBodySystem : loading " << data.size() << " data items...";
        
        // Find density
        auto densityData = data.find("density");
        if (densityData != data.end())
        {
            Scalar density = densityData->second.as<Scalar>();
            LOG(logINFO) << "Density type : " << densityData->second.getType();
            LOG(logINFO) << "Density : " << densityData->second.as<Scalar>();
        }

        // Find static friction
        auto staticFrictionData = data.find("static_friction");
        if (staticFrictionData != data.end())
        {
            LOG(logINFO) << "SFriction type : " << densityData->second.getType();
            LOG(logINFO) << "Static friction : " << staticFrictionData->second.as<Scalar>();
        }

        // Find dynamic friction
        auto dynamicFrictionData = data.find("dynamic_friction");
        if (dynamicFrictionData != data.end())
        {
            LOG(logINFO) << "DFriction type : " << densityData->second.getType();
            LOG(logINFO) << "Dynamic friction : " << dynamicFrictionData->second.as<Scalar>();
        }

        // Find object type
        auto objectTypeData = data.find("object");
        if (objectTypeData != data.end())
        {
            LOG(logINFO) << "Object type : " << densityData->second.getType();
            LOG(logINFO) << "Object type : " << objectTypeData->second.as<std::string>();
        }
    }

    Ra::Engine::Component* RigidBodySystem::addComponentToEntity( Ra::Engine::Entity* entity )
    {
        uint componentId = entity->getComponentsMap().size();

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
