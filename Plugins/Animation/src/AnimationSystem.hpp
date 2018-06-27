#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Engine/System/CouplingSystem.hpp>

#include <AnimationPluginMacros.hpp>
#include <Engine/ItemModel/ItemEntry.hpp>

namespace AnimationPlugin {

/// The AnimationSystem is the main system for coupling TimedSystems.
/// On one hand, it manages the AnimationComponents, i.e. skeleton animation and display.
/// On the other hand, it is responsible for transmitting calls to animation-related systems,
/// for example physics systems that must play with the animation.
class ANIM_PLUGIN_API AnimationSystem : public Ra::Engine::CouplingSystem<Ra::Engine::TimedSystem> {
  public:
    /// Create a new animation system
    AnimationSystem();

    /// Create a task for each animation component to advance the current animation.
    virtual void generateTasks( Ra::Core::TaskQueue* taskQueue,
                                const Ra::Engine::FrameInfo& frameInfo ) override;

    /// Load a skeleton and an animation from a file.
    void handleAssetLoading( Ra::Engine::Entity* entity,
                             const Ra::Asset::FileData* fileData ) override;

    /// Toggle on/off playing of animations.
    void play( bool isPlaying );

    /// Advance the animation next frame, then pauses.
    void step();

    /// Resets the skeleton to its rest pose.
    void reset();

    /// Saves all the state data related to the current frame into a cache file.
    void cacheFrame() const;

    /// Restores the state data related to the \p frameID -th frame from the cache file.
    /// \returns true if the frame has been successfully restored, false otherwise.
    bool restoreFrame( uint frame);

    /// Set on or off xray bone display.
    void setXray( bool on );

    /// Is xray bone display on.
    bool isXrayOn();

    /// Display the skeleton.
    void toggleSkeleton( const bool status );

    /// Set the animation to play.
    void setAnimation( const uint i );

    /// If \p status is TRUE, then use the animation time step if available;
    /// else, use the application timestep.
    void toggleAnimationTimeStep( const bool status );

    /// Set animation speed factor.
    void setAnimationSpeed( const Scalar value );

    /// Toggle the slow motion speed (speed x0.1).
    void toggleSlowMotion( const bool status );

    /// @returns the animation time corresponding to the \p entry 's entity.
    Scalar getTime( const Ra::Engine::ItemEntry& entry ) const;

    /// @returns the system frame.
    uint getAnimFrame() const { return m_animFrame; }

  private:
    /// Current frame
    uint m_animFrame;

    /// See if animation is playing or paused
    bool m_isPlaying;

    /// True if one step has been required to play.
    bool m_oneStep;

    /// True if we want to show xray-bones
    bool m_xrayOn;
};
} // namespace AnimationPlugin

#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
