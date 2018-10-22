#ifndef POSTSUBDIV_PLUGIN_POSTSUBDIV_SYSTEM_HPP_
#define POSTSUBDIV_PLUGIN_POSTSUBDIV_SYSTEM_HPP_

#include <PostSubdivPluginMacros.hpp>

#include <Core/File/FileData.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/System/System.hpp>

#include <PostSubdivComponent.hpp>

namespace PostSubdivPlugin {

class POST_SUBDIV_PLUGIN_API PostSubdivSystem : public Ra::Engine::System {
  public:
    PostSubdivSystem() {}
    virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                const Ra::Engine::FrameInfo& frameInfo ) override {
        for ( const auto& compEntry : m_components )
        {
            auto subdiv = static_cast<PostSubdivComponent*>( compEntry.second );
            auto subdivFunc = std::bind( &PostSubdivComponent::subdiv, subdiv );
            auto subdivTask = new Ra::Core::FunctionTask( subdivFunc, "PostSubdivTask" );
            auto subdivTaskId = taskQueue->registerTask( subdivTask );
            // TODO: find a way to make sure it is one of the last tasks!
        }
    }

    void handleAssetLoading( Ra::Engine::Entity* entity,
                             const Ra::Asset::FileData* fileData ) override {
        auto geomData = fileData->getGeometryData();

        for ( auto geom : geomData )
        {
            auto comp = new PostSubdivComponent( "PostSubdiv_" + geom->getName(),
                                                 PostSubdivComponent::LOOP, 0, entity );
            comp->setContentsName( geom->getName() );
            registerComponent( entity, comp );
        }
    }
};

} // namespace PostSubdivPlugin

#endif // POSTSUBDIV_PLUGIN_POSTSUBDIV_SYSTEM_HPP_
