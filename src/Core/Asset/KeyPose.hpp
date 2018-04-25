#ifndef RADIUMENGINE_KEY_POSE_HPP
#define RADIUMENGINE_KEY_POSE_HPP

#include <Core/Animation/Pose.hpp>
#include <Core/Asset/Interpolation.hpp>
#include <Core/Asset/KeyFrame.hpp>

namespace Ra {
namespace Core {
namespace Asset {

class KeyPose : public KeyFrame<Core::Animation::Pose> {
  public:
    /// CONSTRUCTOR
    KeyPose( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Animation::Pose>( time ) {}
    KeyPose( const KeyPose& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyPose() {}

  protected:
    /// SIZE
    inline uint poseSize() const { return m_keyframe.begin()->second.size(); }

    /// TRANSFORMATION
    inline Core::Animation::Pose defaultFrame() const override {
        return Core::Animation::Pose( poseSize(), Core::Math::Transform::Identity() );
    }

    inline Core::Animation::Pose interpolate( const Core::Animation::Pose& F0,
                                              const Core::Animation::Pose& F1,
                                              const Scalar t ) const override {
        CORE_ASSERT( ( F0.size() == F1.size() ),
                     "Pose size mismatch: why did you put bad poses inside the keyframes?" );
        const uint pose_size = F0.size();
        Animation::Pose result( pose_size );
        for ( uint i = 0; i < pose_size; ++i )
        {
            Asset::interpolate( F0[i], F1[i], t, result[i] );
        }
        return result;
    }
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_POSE_HPP
