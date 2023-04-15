#pragma once

#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Asset/AnimationTime.hpp>
#include <Core/Asset/AssetData.hpp>
#include <Core/RaCore.hpp>

#include <string>
#include <vector>

namespace Ra {
namespace Core {
using namespace Animation;
namespace Asset {

/**
 * A HandleAnimation stores data for an animation Handle.
 */
struct RA_CORE_API HandleAnimation {
    explicit HandleAnimation( const std::string& name = "" ) :
        m_name( name ), m_anim( -1, Transform::Identity() ) {}

    /// The Handle's name.
    std::string m_name;

    /// The list of KeyFramed transforms applied to the Handle.
    KeyFramedValue<Transform> m_anim;

    /// The AnimationTime for the Handle.
    AnimationTime m_animationTime;
};

/**
 * The AnimationData class stores all the HandleAnimation related to an
 * animation of an object, one per animation Handle.
 * \note Objects can have several AnimationData, one for each animation.
 */
class RA_CORE_API AnimationData : public AssetData
{
  public:
    explicit AnimationData( const std::string& name = "" ) :
        AssetData( name ), m_dt( 0.0 ), m_keyFrame() {}

    /// \name Time
    /// \{

    /**
     * \returns the AnimationTime of the animation.
     */
    inline const AnimationTime& getTime() const { return m_time; }

    /**
     * Sets the AnimationTime of the animation.
     */
    inline void setTime( const AnimationTime& time ) { m_time = time; }
    /**
     * \returns the animation time-step.
     */
    inline AnimationTime::Time getTimeStep() const { return m_dt; }

    /**
     * Sets the animation time-step.
    ￼ */
    inline void setTimeStep( const AnimationTime::Time& delta ) { m_dt = delta; }

    /// \}

    /// \name Keyframes
    /// \{

    /**
     * \returns the number of HandleAnimations.
     */
    inline uint getFramesSize() const { return m_keyFrame.size(); }

    /**
     * \returns the list of HandleAnimations, i.e. the whole animation frames.
     */
    inline const std::vector<HandleAnimation>& getHandleData() const { return m_keyFrame; }

    /**
     * Sets the animation frames.
     */
    inline void setHandleData( std::vector<HandleAnimation>&& keyFrames ) {
        m_keyFrame = std::move( keyFrames );
    }
    /// \}

    /**
     * Print stat info to the Debug output.
     */
    void displayInfo() const;

  private:
    /// The AnimationTime for the object.
    AnimationTime m_time;

    /// The animation time-step.
    AnimationTime::Time m_dt { 0 };

    /// The animation frames.
    std::vector<HandleAnimation> m_keyFrame;
};

} // namespace Asset
} // namespace Core
} // namespace Ra
