#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Core/Animation/AnimationTime.hpp>
#include <Core/Animation/KeyTransform.hpp>
#include <Core/Asset/AssetData.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/**
 * A HandleAnimation stores the animation KeyFrames of transforms applied to
 * a Handle, e.g.\ a bone of an animation skeleton.
 */
struct RA_CORE_API HandleAnimation {
    HandleAnimation( const std::string& name = "" );

    /// The Handle's name.
    std::string m_name;
    /// The list of KeyFramed transformed applied to the Handle.
    Core::Animation::KeyTransform m_anim;
};

/**
 * The AnimationData class stores all the HandleAnimation related to an
 * animation of an object, e.g.\ the HandleAnimation of all the animation
 * skeleton bones plus its own.
 * \note Objects can have several AnimationData, one for each animation.
 */
class RA_CORE_API AnimationData : public AssetData {
  public:
    AnimationData( const std::string& name = "" );

    ~AnimationData() override;

    /**
     * Set the name of the object.
     */
    inline void setName( const std::string& name );

    /// \name Time
    /// \{

    /**
     * Return the AnimationTime of the animation.
     */
    inline const Core::Animation::AnimationTime& getTime() const;

    /**
     * Set the AnimationTime of the animation.
     */
    inline void setTime( const Core::Animation::AnimationTime& time );

    /**
     * Return the animation timestep.
     */
    inline Core::Animation::Time getTimeStep() const;

    /**
     * Set the animation timestep.
     */
    inline void setTimeStep( const Core::Animation::Time& delta );
    /// \}

    /// \name KeyFrames
    /// \{

    /**
     * Return the number of HandleAnimations.
     */
    inline uint getFramesSize() const;

    /**
     * Return the list of HandleAnimations, i.e.\ the whole animation frames.
     */
    inline std::vector<HandleAnimation> getFrames() const;

    /**
     * Set the animation frames.
     */
    inline void setFrames( const std::vector<HandleAnimation>& frameList );
    /// \}

    /**
     * Print stat info to the Debug output.
     */
    inline void displayInfo() const;

  protected:
    /// The AnimationTime for the object.
    Core::Animation::AnimationTime m_time;

    /// The animation timestep.
    Core::Animation::Time m_dt;

    /// The animation frames.
    std::vector<HandleAnimation> m_keyFrame;
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#include <Core/Asset/AnimationData.inl>

#endif // RADIUMENGINE_ANIMATION_DATA_HPP
