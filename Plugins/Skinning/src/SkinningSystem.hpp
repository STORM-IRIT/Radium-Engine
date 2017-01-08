#ifndef SKINPLUGIN_SKINNING_SYSTEM_HPP_
#define SKINPLUGIN_SKINNING_SYSTEM_HPP_

#include <SkinningPlugin.hpp>

#include <Engine/System/System.hpp>

#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Tasks/Task.hpp>

#include <Engine/Assets/FileData.hpp>
#include <Engine/Assets/HandleData.hpp>
#include <Engine/Entity/Entity.hpp>
#include <SkinningComponent.hpp>

#include <Display/SkinningDisplayComponent.hpp>

namespace SkinningPlugin
{

    class SKIN_PLUGIN_API SkinningSystem :  public Ra::Engine::System
    {
    public:
        SkinningSystem(){}
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                    const Ra::Engine::FrameInfo& frameInfo ) override
        {
            for (const auto& compEntry : m_components)
            {
                SkinningComponent* comp = static_cast<SkinningComponent*>(compEntry.second);
                Ra::Core::FunctionTask* skinTask = new Ra::Core::FunctionTask(
                        std::bind(&SkinningComponent::skin, comp),
                        "SkinnerTask"
                );

                Ra::Core::FunctionTask* endTask = new Ra::Core::FunctionTask(
                        std::bind(&SkinningComponent::endSkinning, comp),
                        "SkinnerEndTask"
                );

                Ra::Core::TaskQueue::TaskId skinTaskId = taskQueue->registerTask(skinTask);
                Ra::Core::TaskQueue::TaskId endTaskId = taskQueue->registerTask(endTask);
                taskQueue->addPendingDependency( "AnimatorTask", skinTaskId );
                taskQueue->addDependency( skinTaskId, endTaskId);
            }

        }

        void handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData) override
        {

            auto geomData = fileData->getGeometryData();
            auto skelData = fileData->getHandleData();

            if ( geomData.size() > 0  && skelData.size() > 0 )
            {
                for (const auto& skel : skelData)
                {
                    SkinningComponent* component = new SkinningComponent("SkC_" + skel->getName());
                    entity->addComponent( component );
                    component->handleWeightsLoading(skel);
                    registerComponent(entity, component);

                    SkinningDisplayComponent* display = new SkinningDisplayComponent("SkC_DSP_" + skel->getName(), skel->getName() );
                    entity->addComponent( display );
                    //display->display( component->getRefData() );
                }
            }
        }

    };
}


#endif // ANIMPLUGIN_SKINNING_SYSTEM_HPP_
