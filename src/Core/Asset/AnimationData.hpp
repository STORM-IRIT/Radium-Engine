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

/// A HandleAnimation stores the animation KeyFrames of transforms applied to
/// a part of an object, e.g. a bone of an animation skeleton.
struct RA_CORE_API HandleAnimation {
    explicit HandleAnimation( const std::string& name = "" );

    /// The handle's name.
    std::string m_name;

    /// The list of KeyFramed transformed applied to the handle.
    Core::Animation::KeyTransform m_anim;
};

/// The AnimationData class stores all the HandleAnimations related to an object,
/// e.g. the ones from all the skeleton bones.
class RA_CORE_API AnimationData : public AssetData
{
  public:
    explicit AnimationData( const std::string& name = "" );

    ~AnimationData();

    /// Returns the name of the object.
    inline void setName( const std::string& name );

    /// Return the AnimationTime linked to the object.
    inline const Core::Animation::AnimationTime& getTime() const;

    /// Set the AnimationTime for the object.
    inline void setTime( const Core::Animation::AnimationTime& time );

    /// Return the animation timestep.
    inline Core::Animation::Time getTimeStep() const;

    /// Set the animation timestep.
    inline void setTimeStep( const Core::Animation::Time& delta );

    /// Return the number of transform.
    inline uint getFramesSize() const;

    /// Return the HandleAnimation, i.e. the whole animation frames.
    inline std::vector<HandleAnimation> getFrames() const;

    /// Set the animation frames.
    inline void setFrames( const std::vector<HandleAnimation>& frameList );

    /// Print stat info to the Debug output.
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
