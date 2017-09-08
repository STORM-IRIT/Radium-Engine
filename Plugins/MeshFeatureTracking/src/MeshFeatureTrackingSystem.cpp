#include <MeshFeatureTrackingSystem.hpp>

#include <string>
#include <iostream>

#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Tasks/Task.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/FrameInfo.hpp>

namespace MeshFeatureTrackingPlugin
{
    MeshFeatureTrackingSystem::MeshFeatureTrackingSystem()
    {
    }

    void MeshFeatureTrackingSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {
    }
}
