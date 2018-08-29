#ifndef RADIUMENGINE_KEY_TRANSFORM_HPP
#define RADIUMENGINE_KEY_TRANSFORM_HPP

#include <Core/File/KeyFrame/Interpolation.hpp>
#include <Core/File/KeyFrame/KeyFrame.hpp>

namespace Ra {
namespace Asset {

/// The KeyTransform class is a list of KeyFramed transformation matrices.
class KeyTransform : public KeyFrame<Core::Transform> {
  public:
    KeyTransform( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Transform>( time ) {}

    KeyTransform( const KeyTransform& keyframe ) = default;

    ~KeyTransform() {}

  protected:
    inline Core::Transform defaultFrame() const override { return Core::Transform::Identity(); }

    inline Core::Transform interpolate( const Core::Transform& F0, const Core::Transform& F1,
                                        const Scalar t ) const override {
        Core::Transform result;
        Core::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_KEY_TRANSFORM_HPP
