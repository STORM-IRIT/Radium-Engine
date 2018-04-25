#ifndef SKINPLUGIN_SKINNING_SYSTEM_HPP_
#define SKINPLUGIN_SKINNING_SYSTEM_HPP_

#include <SkinningPluginMacros.hpp>

#include <Engine/System/System.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Utils/Task.hpp>
#include <Core/Utils/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <SkinningComponent.hpp>

#include <Display/SkinningDisplayComponent.hpp>

namespace SkinningPlugin {

class SKIN_PLUGIN_API SkinningSystem : public Ra::Engine::System {
  public:
    SkinningSystem() {}
    virtual void generateTasks( Ra::Core::Utils::TaskQueue* taskQueue,
                                const Ra::Engine::FrameInfo& frameInfo ) override {
        for ( const auto& compEntry : m_components )
        {
            SkinningComponent* comp = static_cast<SkinningComponent*>( compEntry.second );
            Ra::Core::Utils::FunctionTask* skinTask = new Ra::Core::Utils::FunctionTask(
                std::bind( &SkinningComponent::skin, comp ), "SkinnerTask" );

            Ra::Core::Utils::FunctionTask* endTask = new Ra::Core::Utils::FunctionTask(
                std::bind( &SkinningComponent::endSkinning, comp ), "SkinnerEndTask" );

            Ra::Core::Utils::TaskQueue::TaskId skinTaskId = taskQueue->registerTask( skinTask );
            Ra::Core::Utils::TaskQueue::TaskId endTaskId = taskQueue->registerTask( endTask );
            taskQueue->addPendingDependency( "AnimatorTask", skinTaskId );
            taskQueue->addDependency( skinTaskId, endTaskId );
        }
    }

    void handleAssetLoading( Ra::Engine::Entity* entity,
                             const Ra::Core::Asset::FileData* fileData ) override {

        auto geomData = fileData->getGeometryData();
        auto skelData = fileData->getHandleData();

        if ( geomData.size() > 0 && skelData.size() > 0 )
        {
            for ( const auto& skel : skelData )
            {
                SkinningComponent* component = new SkinningComponent(
                    "SkC_" + skel->getName(), SkinningComponent::LBS, entity );
                component->handleWeightsLoading( skel );
                registerComponent( entity, component );

                SkinningDisplayComponent* display = new SkinningDisplayComponent(
                    "SkC_DSP_" + skel->getName(), skel->getName(), entity );
                // display->display( component->getRefData() );
            }
        }
    }
};
} // namespace SkinningPlugin

#endif // ANIMPLUGIN_SKINNING_SYSTEM_HPP_
