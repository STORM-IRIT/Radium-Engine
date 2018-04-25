#ifndef RADIUMENGINE_KEY_TRANSFORM_HPP
#define RADIUMENGINE_KEY_TRANSFORM_HPP

#include <Core/Asset/Interpolation.hpp>
#include <Core/Asset/KeyFrame.hpp>

namespace Ra {
namespace Core {
namespace Asset {

class KeyTransform : public KeyFrame<Core::Math::Transform> {
  public:
    /// CONSTRUCTOR
    KeyTransform( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Math::Transform>( time ) {}
    KeyTransform( const KeyTransform& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyTransform() {}

  protected:
    /// TRANSFORMATION
    inline Core::Math::Transform defaultFrame() const override { return Core::Math::Transform::Identity(); }

    inline Core::Math::Transform interpolate( const Core::Math::Transform& F0, const Core::Math::Transform& F1,
                                        const Scalar t ) const override {
        Core::Math::Transform result;
        Core::Asset::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_TRANSFORM_HPP
