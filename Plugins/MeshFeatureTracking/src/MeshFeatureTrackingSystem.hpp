#ifndef MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_SYSTEM_HPP_
#define MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_SYSTEM_HPP_

#include <Engine/System/System.hpp>

#include <MeshFeatureTrackingPluginMacros.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>


namespace MeshFeatureTrackingPlugin
{
    class MESH_FEATURE_TRACKING_PLUGIN_API MeshFeatureTrackingSystem :  public Ra::Engine::System
    {
    public:

        /// Create a new MeshFeatureTracking system
        MeshFeatureTrackingSystem();

        /// Create a task for each MeshFeatureTracking component to advance the current MeshFeatureTracking.
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                    const Ra::Engine::FrameInfo& frameInfo ) override;
    };
}


#endif // MESHFEATURETRACKINGPLUGIN_MESHFEATURETRACKING_SYSTEM_HPP_
