#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Engine/System/System.hpp>

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

        /// Set on or off xray bone display.
        void setXray( bool on );

        bool isXrayOn();

        void toggleSkeleton( const bool status );
        void setAnimation( const uint i );
        void toggleAnimationTimeStep( const bool status );
        void setAnimationSpeed( const Scalar value );
        void toggleSlowMotion( const bool status );

    private:
        bool m_isPlaying; /// See if animation is playing or paused
        bool m_oneStep;   /// True if one step has been required to play.
        bool m_xrayOn;    /// True if we want to show xray-bones
    };
}


#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
