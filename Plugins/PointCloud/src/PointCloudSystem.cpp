#include <PointCloudSystem.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/GeometryData.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <PointCloudComponent.hpp>

namespace PointCloudPlugin
{

    PointCloudSystem::PointCloudSystem()
            : Ra::Engine::System()
    {
    }

    PointCloudSystem::~PointCloudSystem()
    {
    }

    void PointCloudSystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        // Do nothing, as this system only displays meshes.
    }
    void PointCloudSystem::setNeighSize(int size)
    {
        LOG(logINFO) << "Neigh Size: " << size;

    }
    void PointCloudSystem::setUseNormal(bool useNormal)
    {
        LOG(logINFO) << "Using Normal: " << useNormal;
    }
    void PointCloudSystem::setDepthThresh(double dThresh)
    {
        LOG(logINFO) << "Depth Threshold: " << dThresh;
    }

} // namespace PointCloudPlugin
