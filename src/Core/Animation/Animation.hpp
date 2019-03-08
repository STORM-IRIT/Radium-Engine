#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <Core/Animation/Pose.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * An Animation is a set of key Poses given for certain time values.
 */
class RA_CORE_API Animation {
  public:
    using MyKeyPose = std::pair<Scalar, Pose>;

    /**
     * Register the given key Pose at the given time value.
     * \note normalize() must be called afterwards to ensure consistency.
     * \warning \p timestamp must be given in seconds.
     */
    void addKeyPose( const Pose& pose, Scalar timestamp );

    /**
     * Register the given key Pose.
     * \note normalize() must be called afterwards to ensure consistency.
     */
    void addKeyPose( const MyKeyPose& keyPose );

    /**
     * Remove all the key poses.
     */
    void clear();

    /**
     * Return true if there is no key Pose in the animation, false otherwise.
     */
    bool isEmpty() const;

    /**
     * Re-order the Poses by chronological order.
     */
    void normalize();

    /**
     * Return the Pose corresponding to the given timestamp.
     * \warning timestamp must be given in seconds.
     */
    Pose getPose( Scalar timestamp ) const;

    /**
     * Return the internal animation time from \p timestamp,
     * guaranteed to be between 0 and the animation last time.
     * \warning timestamp must be given in seconds.
     */
    Scalar getTime( Scalar timestamp ) const;

    /**
     * Returns the animation last time in seconds.
     */
    Scalar getDuration() const;

  private:
    /// The list of key Poses.
    std::vector<MyKeyPose> m_keys;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // ANIMATION_HPP
