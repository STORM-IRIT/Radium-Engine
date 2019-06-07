#ifndef RADIUMENGINE_KEY_ROTATION_HPP
#define RADIUMENGINE_KEY_ROTATION_HPP

#include <Core/Animation/Interpolation.hpp>
#include <Core/Animation/KeyFrame.hpp>

namespace Ra {
namespace Core {
namespace Animation {

class KeyRotation : public KeyFrame<Core::Quaternion>
{
  public:
    /// CONSTRUCTOR
    explicit KeyRotation( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Quaternion>( time ) {}
    KeyRotation( const KeyRotation& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyRotation() {}

  protected:
    /// TRANSFORMATION
    inline Core::Quaternion defaultFrame() const override {
        return Core::Quaternion( 1.0, 0.0, 0.0, 0.0 );
    }

    inline Core::Quaternion interpolate( const Core::Quaternion& F0,
                                         const Core::Quaternion& F1,
                                         const Scalar t ) const override {
        Core::Quaternion result;
        Core::Animation::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_ROTATION_HPP
