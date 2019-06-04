#ifndef RADIUMENGINE_KEY_POSE_HPP
#define RADIUMENGINE_KEY_POSE_HPP

#include <Core/Animation/Interpolation.hpp>
#include <Core/Animation/KeyFrame.hpp>
#include <Core/Animation/Pose.hpp>

namespace Ra {
namespace Core {
namespace Animation {

class KeyPose : public KeyFrame<Pose> {
  public:
    /// CONSTRUCTOR
    explicit KeyPose( const AnimationTime& time = AnimationTime() ) : KeyFrame<Pose>( time ) {}
    KeyPose( const KeyPose& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyPose() {}

  protected:
    /// SIZE
    inline uint poseSize() const { return m_keyframe.begin()->second.size(); }

    /// TRANSFORMATION
    inline Pose defaultFrame() const override {
        return Pose( poseSize(), Core::Transform::Identity() );
    }

    inline Pose interpolate( const Pose& F0, const Pose& F1, const Scalar t ) const override {
        CORE_ASSERT( ( F0.size() == F1.size() ),
                     "Pose size mismatch: why did you put bad poses inside the keyframes?" );
        const uint pose_size = F0.size();
        Pose result( pose_size );
        for ( uint i = 0; i < pose_size; ++i )
        {
            Core::Animation::interpolate( F0[i], F1[i], t, result[i] );
        }
        return result;
    }
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_POSE_HPP
