#ifndef SKINPLUGIN_SKINNING_SYSTEM_HPP_
#define SKINPLUGIN_SKINNING_SYSTEM_HPP_

#include <SkinningPluginMacros.hpp>

#include <Engine/System/System.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <SkinningComponent.hpp>

#include <Display/SkinningDisplayComponent.hpp>

namespace SkinningPlugin {

class SKIN_PLUGIN_API SkinningSystem : public Ra::Engine::System {
  public:
    SkinningSystem() {}
    virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                const Ra::Engine::FrameInfo& frameInfo ) override {
        for ( const auto& compEntry : m_components )
        {
            SkinningComponent* comp = static_cast<SkinningComponent*>( compEntry.second );
            Ra::Core::FunctionTask* skinTask = new Ra::Core::FunctionTask(
                std::bind( &SkinningComponent::skin, comp ), "SkinnerTask" );

            Ra::Core::FunctionTask* endTask = new Ra::Core::FunctionTask(
                std::bind( &SkinningComponent::endSkinning, comp ), "SkinnerEndTask" );

            Ra::Core::TaskQueue::TaskId skinTaskId = taskQueue->registerTask( skinTask );
            Ra::Core::TaskQueue::TaskId endTaskId = taskQueue->registerTask( endTask );
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
            for ( const auto& geom : geomData )
            {
                // look for a skeleton skining this mesh (!should be at most one such skeleton!)
                auto it =
                    std::find_if( skelData.begin(), skelData.end(), [&geom]( const auto& skel ) {
                        return std::find_if( skel->getBindMeshes().begin(),
                                             skel->getBindMeshes().end(),
                                             [&geom]( const auto& meshName ) {
                                                 return meshName == geom->getName();
                                             } ) != skel->getBindMeshes().end();
                    } );
                if ( it != skelData.end() )
                {
                    const auto& skel = *it;
                    SkinningComponent* component = new SkinningComponent(
                        "SkC_" + geom->getName(), SkinningComponent::LBS, entity );
                    component->handleWeightsLoading( skel, geom->getName() );
                    registerComponent( entity, component );
                    new SkinningDisplayComponent( "SkC_DSP_" + geom->getName(), geom->getName(),
                                                  entity );
                }
            }
        }
    }

    void showWeights( bool on ) {
        for ( auto& compEntry : m_components )
        {
            static_cast<SkinningComponent*>( compEntry.second )->showWeights( on );
        }
    }

    void showWeightsType( int type ) {
        for ( auto& compEntry : m_components )
        {
            static_cast<SkinningComponent*>( compEntry.second )->showWeightsType( type );
        }
    }
};
} // namespace SkinningPlugin

#endif // ANIMPLUGIN_SKINNING_SYSTEM_HPP_
