#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Engine/Entity/System.hpp>

#include <AnimationPlugin.hpp>

namespace AnimationPlugin
{
    class ANIM_PLUGIN_API AnimationSystem :  public Ra::Engine::System
    {
    public:

        /// Create a new animation system
        AnimationSystem();

        /// Create a task for each animation component to advance the current animation.
        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                    const Ra::Engine::FrameInfo& frameInfo ) override;

        /// Load a skeleton and an animation from a file.
        void handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData) override;

        /// Toggle on/off playing of animations.
        void setPlaying( bool isPlaying );
        
        /// Advance the animation next frame, then pauses. 
        void step();

        /// Resets the skeleton to its rest pose.
        void reset();

    private:
        bool m_isPlaying; /// See if animation is playing or paused
        bool m_oneStep;   /// True if one step has been required to play.
    };
}


#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
