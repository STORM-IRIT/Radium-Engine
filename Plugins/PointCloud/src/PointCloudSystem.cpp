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
#include <Engine/Renderer/Renderers/TempRenderer.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <PointCloudComponent.hpp>
#include <GuiBase/Viewer/Viewer.hpp>
#include <MainApplication.hpp>
#include <Gui/MainWindow.hpp>


namespace PointCloudPlugin
{

    PointCloudSystem::PointCloudSystem()
            : Ra::Engine::System()
    {
        m_renderer = static_cast<Ra::Engine::TempRenderer*>(const_cast<Ra::Engine::Renderer*>(mainApp->m_mainWindow->getViewer()->getRenderer()));
    }

    PointCloudSystem::~PointCloudSystem()
    {
    }

    void PointCloudSystem::generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo )
    {
        // Do nothing
    }
 /*   void PointCloudSystem::handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData )
    {
        auto geomData = fileData->getGeometryData();

        uint id = 0;

        for ( const auto& data : geomData )
        {
            std::string componentName = "PCC_" + entity->getName() + std::to_string( id++ );
            PointCloudComponent * comp = new PointCloudComponent( componentName );
            entity->addComponent( comp );
            comp->handleDataLoading(data);
            registerComponent( entity, comp );
        }
    }
   */ void PointCloudSystem::setNeighSize(int size)
    {
        LOG(logINFO) << "Neigh Size: " << size;
        m_renderer->setNeighSize(size);
    }
    void PointCloudSystem::setUseNormal(bool useNormal)
    {
        LOG(logINFO) << "Using Normal: " << useNormal;
        m_renderer->setUseNormal(useNormal);
    }
    void PointCloudSystem::setDepthThresh(double dThresh)
    {
        LOG(logINFO) << "Depth Threshold: " << dThresh;
        m_renderer->setDepthThresh(dThresh);
    }

} // namespace PointCloudPlugin
