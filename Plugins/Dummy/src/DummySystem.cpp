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
        m_data = new DummyData;
    }

    DummySystem::~DummySystem()
    {
        delete m_data;
    }

    void DummySystem::initialize()
    {
        m_data->foo = 42;
        m_data->bar = 1337;
    }

    void DummySystem::handleDataLoading( Ra::Engine::Entity* entity, const std::string& rootFolder,
                                             const std::map<std::string, Ra::Core::Any>& data )
    {
    }

    Ra::Engine::Component* DummySystem::addComponentToEntityInternal( Ra::Engine::Entity* entity, uint id )
    {
        std::string componentName = "DummyComponent_" + entity->getName() + std::to_string( id );
        DummyComponent* component = new DummyComponent( componentName );

        return component;
    }

    void DummySystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        auto task1Id = taskQueue->registerTask( task1 );
        auto task2Id = taskQueue->registerTask( task2 );
    }

} // namespace DummyPlugin
