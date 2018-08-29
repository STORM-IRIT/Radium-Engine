#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <Core/Animation/Pose/Pose.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Animation {

using KeyPose = std::pair<Scalar, Pose>;

/// An Animation is a set of key Poses given for certain time values.
class RA_CORE_API Animation {
  public:
    /// Add the key Pose after the previous ones.
    /// \note normalize() must be called afterwards to ensure consistency.
    /// \warning \p timestamp must be given in seconds.
    void addKeyPose( const Pose& pose, Scalar timestamp );
    void addKeyPose( const KeyPose& keyPose );

    /// Remove all the key Poses.
    void clear();

    /// Return true if there is no key Pose in the animation.
    bool isEmpty() const;

    /// Re-order the Poses by chronological order.
    void normalize();

    /// Return the Pose corresponding to the given timestamp.
    /// \warning timestamp must be given in seconds.
    Pose getPose( Scalar timestamp ) const;

    /// Return the internal animation time from \p timestamp,
    /// guaranteed to be between 0 and the animation last time.
    /// \warning timestamp must be given in seconds.
    Scalar getTime( Scalar timestamp ) const;

  private:
    /// The list of key Poses.
    std::vector<KeyPose> m_keys;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // ANIMATION_HPP
