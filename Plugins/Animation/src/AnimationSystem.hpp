#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Engine/Entity/System.hpp>

#include "AnimationPlugin.hpp"

namespace AnimationPlugin
{
    class ANIM_PLUGIN_API AnimationSystem :  public Ra::Engine::System
    {
    public:
        AnimationSystem();
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                    const Ra::Engine::FrameInfo& frameInfo ) override;
        void handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData) override;
        void setPlaying( bool isPlaying );
        void step();
        void reset();

    protected:
        virtual Ra::Engine::Component* addComponentToEntityInternal(
                Ra::Engine::Entity* entity, uint id ) override;
        virtual void callbackOnComponentCreation( const Ra::Engine::Component *component ) override;
    private:
        bool m_isPlaying;
        bool m_oneStep;
    };
}


#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
