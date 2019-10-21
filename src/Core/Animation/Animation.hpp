#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <Core/Animation/Pose.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Animation {

/// An Animation is a set of key Poses given for certain time values.
/// This class is tagged as deprecated and will soon be replaced by a
/// finalized version of Ra::Core::Keyframe, providing more general handling of KeyFramed data.
class RA_CORE_API [[deprecated]] Animation
{
  public:
    /// The type for animation keyPose.
    using MyKeyPose = std::pair<Scalar, Pose>;

    /// Add the key Pose at the given time.
    /// \note normalize() must be called afterwards to ensure consistency.
    /// \warning \p timestamp must be given in seconds.
    void addKeyPose( const Pose& pose, Scalar timestamp );

    /// Add the given key Pose.
    /// \note normalize() must be called afterwards to ensure consistency.
    void addKeyPose( const MyKeyPose& keyPose );

    /// Remove all the key Poses.
    void clear();

    /// Return true if there is no key Pose in the animation.
    bool isEmpty() const;

    /// Re-order the poses by chronological order.
    void normalize();

    /// Get the pose corresponding to the given timestamp.
    /// \warning \p timestamp must be given in seconds.
    Pose getPose( Scalar timestamp ) const;

    /// Get the internal animation time from a timestamp.
    /// Guaranteed to be between 0 and the animation last time.
    /// \warning \p timestamp must be given in seconds.
    Scalar getTime( Scalar timestamp ) const;

    /// Get the animation last time.
    Scalar getDuration() const;

  private:
    /// The list of key Poses.
    std::vector<MyKeyPose> m_keys;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // ANIMATION_HPP
