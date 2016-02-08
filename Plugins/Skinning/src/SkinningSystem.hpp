#ifndef SKINPLUGIN_SKINNING_SYSTEM_HPP_
#define SKINPLUGIN_SKINNING_SYSTEM_HPP_

#include <Engine/Entity/System.hpp>

#include <SkinningPlugin.hpp>

namespace SkinningPlugin
{
    class SKIN_PLUGIN_API SkinningSystem :  public Ra::Engine::System
    {
    public:
        SkinningSystem(){}
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                    const Ra::Engine::FrameInfo& frameInfo ) override {}
        void handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData) override {}
    };
}


#endif // ANIMPLUGIN_SKINNING_SYSTEM_HPP_
