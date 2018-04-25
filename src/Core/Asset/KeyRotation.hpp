#ifndef RADIUMENGINE_KEY_ROTATION_HPP
#define RADIUMENGINE_KEY_ROTATION_HPP

#include <Core/Asset/Interpolation.hpp>
#include <Core/Asset/KeyFrame.hpp>

namespace Ra {
namespace Core {
namespace Asset {

class KeyRotation : public KeyFrame<Core::Math::Quaternion> {
  public:
    /// CONSTRUCTOR
    KeyRotation( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Math::Quaternion>( time ) {}
    KeyRotation( const KeyRotation& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyRotation() {}

  protected:
    /// TRANSFORMATION
    inline Core::Math::Quaternion defaultFrame() const override {
        return Core::Math::Quaternion( 1.0, 0.0, 0.0, 0.0 );
    }

    inline Core::Math::Quaternion interpolate( const Core::Math::Quaternion& F0, const Core::Math::Quaternion& F1,
                                         const Scalar t ) const override {
        Core::Math::Quaternion result;
        Core::Asset::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_ROTATION_HPP
