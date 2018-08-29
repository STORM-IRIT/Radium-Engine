#ifndef RADIUMENGINE_KEY_ROTATION_HPP
#define RADIUMENGINE_KEY_ROTATION_HPP

#include <Core/File/KeyFrame/Interpolation.hpp>
#include <Core/File/KeyFrame/KeyFrame.hpp>

namespace Ra {
namespace Asset {

/// The class KeyRotation is a list of KeyFramed rotations expressed as Quaternions.
class KeyRotation : public KeyFrame<Core::Quaternion> {
  public:
    KeyRotation( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Quaternion>( time ) {}

    KeyRotation( const KeyRotation& keyframe ) = default;

    ~KeyRotation() {}

  protected:
    inline Core::Quaternion defaultFrame() const override {
        return Core::Quaternion( 1.0, 0.0, 0.0, 0.0 );
    }

    inline Core::Quaternion interpolate( const Core::Quaternion& F0, const Core::Quaternion& F1,
                                         const Scalar t ) const override {
        Core::Quaternion result;
        Core::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_KEY_ROTATION_HPP
