#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Core/File/AssetData.hpp>
#include <Core/File/KeyFrame/AnimationTime.hpp>
#include <Core/File/KeyFrame/KeyTransform.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Asset {

/// A HandleAnimation stores the animation KeyFrames of transforms applied to
/// a part of an object, e.g. a bone of an animation skeleton.
struct RA_CORE_API HandleAnimation {
    HandleAnimation( const std::string& name = "" );

    /// The object's name.
    std::string m_name;

    /// The list of KeyFramed transformed applied to the object.
    KeyTransform m_anim;
};

/// The AnimationData class stores all the HandleAnimation related to an object,
/// e.g. the HandleAnimation of all the animation skeleton bones plus its own.
class RA_CORE_API AnimationData : public AssetData {
  public:
    AnimationData( const std::string& name = "" );

    ~AnimationData();

    /// Returns the name of the object.
    inline void setName( const std::string& name );

    /// Return the AnimationTime linked to the object.
    inline const AnimationTime& getTime() const;

    /// Set the AnimationTime for the object.
    inline void setTime( const AnimationTime& time );

    /// Return the animation timestep.
    inline Time getTimeStep() const;

    /// Set the animation timestep.
    inline void setTimeStep( const Time& delta );

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
    AnimationTime m_time;

    /// The animation timestep.
    Time m_dt;

    /// The animation frames.
    std::vector<HandleAnimation> m_keyFrame;
};

} // namespace Asset
} // namespace Ra

#include <Core/File/AnimationData.inl>

#endif // RADIUMENGINE_ANIMATION_DATA_HPP
