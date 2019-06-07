#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <Core/Animation/Pose.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Animation {

class RA_CORE_API Animation
{
  public:
    using MyKeyPose = std::pair<Scalar, Pose>;
    // Add the key pose after the previous ones.
    // Call normalize after all the key poses have been added.
    // timestamp must be given in seconds.
    void addKeyPose( const Pose& pose, Scalar timestamp );
    void addKeyPose( const MyKeyPose& keyPose );

    // Remove all the key poses.
    void clear();

    bool isEmpty() const;

    // Re-order the poses by chronological order.
    void normalize();

    // Get the pose corresponding to the given timestamp.
    // timestamp must be given in seconds.
    Pose getPose( Scalar timestamp ) const;

    // Get the internal animation time from a timestamp.
    // Guaranteed to be between 0 and the animation last time
    Scalar getTime( Scalar timestamp ) const;

    // Get the animation last time.
    Scalar getDuration() const;

  private:
    std::vector<MyKeyPose> m_keys;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // ANIMATION_HPP
