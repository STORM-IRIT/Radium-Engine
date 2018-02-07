#include <Engine/Managers/LightManager/LightManager.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>

#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/File/FileData.hpp>
#include <Core/File/GeometryData.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

namespace Ra {
    namespace Engine {

        LightManager::LightManager()
        : m_data( nullptr )
        {
        }

        LightManager::~LightManager()
        {
        }

        void LightManager::generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo )
        {
            /*
            futur work for async rendering
            Ra::Core::FunctionTask* preprocess_task = new Ra::Core::FunctionTask(
                std::bind(&LightManager::preprocess, this),
                "PreProcessLight"
            );
            
            Ra::Core::TaskQueue::TaskId preprocess_id = taskQueue->registerTask( preprocess_task );
            Ra::Core::TaskQueue::TaskId lastprocess_id = preprocess_id;
                
            for (const auto& compEntry : m_components)
            {
                Light* comp = static_cast<Light*>( compEntry.second );
                
                if(!comp)
                    continue;
                
                Ra::Core::FunctionTask* prerender_task = new Ra::Core::FunctionTask(
                   [this,comp]() { this->prerender(*comp); }, "PreRenderLight"
                );

                Ra::Core::FunctionTask* postrender_task = new Ra::Core::FunctionTask(
                   [this,comp]() { this->postrender(*comp); }, "PostRenderLight"
                );

                Ra::Core::TaskQueue::TaskId prerender_id = taskQueue->registerTask( prerender_task );
                Ra::Core::TaskQueue::TaskId postrender_id = taskQueue->registerTask( postrender_task );
                
                taskQueue->addDependency( preprocess_id, prerender_id);
                taskQueue->addDependency( prerender_id, postrender_id);
            
                lastprocess_id = postrender_id;
            }
            
            Ra::Core::FunctionTask* postprocess_task = new Ra::Core::FunctionTask(
                std::bind(&LightManager::postprocess, this),
                "PostProcess"
            );
            
            Ra::Core::TaskQueue::TaskId postprocess_id = taskQueue->registerTask( postprocess_task );
            
            taskQueue->addDependency( postprocess_id, lastprocess_id);
            */
        }
    
        void LightManager::handleAssetLoading( Entity* entity, const Asset::FileData* fileData )
        {
            std::vector<Asset::LightData*> lightData = fileData->getLightData();
            uint id = 0;

            for(const auto& data : lightData)
            {
                std::string componentName = "LIGHT_" + entity->getName() + std::to_string( id++ );
                Light * comp = nullptr;
                
                switch(data->getType())
                {
                    case Asset::LightData::POINT_LIGHT:
                        comp = new PointLight();
                    break;
                    case Asset::LightData::SPOT_LIGHT:
                        comp = new SpotLight();
                    break;
                    case Asset::LightData::DIRECTIONAL_LIGHT:
                        comp = new DirectionalLight();
                    break;
                    case Asset::LightData::AREA_LIGHT:
                        //! PolygonalLight inside PBRPlugin
                        //! TODO not yet
                    break;
                    default:
                        comp = nullptr;
                }
                
                //! @comp should be allocated in LightStorage
                
                if(!comp)
                    continue;
                
                entity->addComponent( comp );
                registerComponent( entity, comp );
            }
        }
    }
}
