#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Engine/System/System.hpp>

#include <AnimationPluginMacros.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>

namespace AnimationPlugin {

/// The AnimationSystem manages the AnimationComponents, i.e. skeleton animation and display.
class ANIM_PLUGIN_API AnimationSystem : public Ra::Engine::System {
  public:
    AnimationSystem();

    /// Create a task for each animation component to advance the current animation.
    void generateTasks( Ra::Core::TaskQueue* taskQueue,
                        const Ra::Engine::FrameInfo& frameInfo ) override;

    /// Load a skeleton and an animation from a file.
    void handleAssetLoading( Ra::Engine::Entity* entity,
                             const Ra::Asset::FileData* fileData ) override;

    /// Toggle on/off playing of animations.
    void setPlaying( bool isPlaying );

    /// Advance the animation next frame, then pauses.
    void step();

    /// Resets the skeleton to its rest pose.
    void reset();

    /// Set on or off xray bone display.
    void setXray( bool on );

    /// @returns whether bone xray is active or not.
    bool isXrayOn();

    /// Toggle on/off display of animation skeletons.
    void toggleSkeleton( const bool status );

    /// Sets the current animation.
    void setAnimation( const uint i );

    /// Toggle on/off using the time step of the animation.
    void toggleAnimationTimeStep( const bool status );

    /// Set the speed factor to play the animation.
    void setAnimationSpeed( const Scalar value );

    /// Toggle on/off using slow motion (i.e. adding a 0.1 factor to the animation speed).
    void toggleSlowMotion( const bool status );

    /// @returns the animation time for the \p entry object.
    Scalar getTime( const Ra::Engine::ItemEntry& entry ) const;

  private:
    /// See if animation is playing or paused
    bool m_isPlaying;

    /// True if one step has been required to play.
    bool m_oneStep;

    /// True if we want to show xray-bones
    bool m_xrayOn;
};
} // namespace AnimationPlugin

#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
