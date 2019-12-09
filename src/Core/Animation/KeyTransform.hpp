#ifndef RADIUMENGINE_KEY_TRANSFORM_HPP
#define RADIUMENGINE_KEY_TRANSFORM_HPP

#include <Core/Animation/Interpolation.hpp>
#include <Core/Animation/KeyFrame.hpp>

namespace Ra {
namespace Core {
namespace Animation {

class KeyTransform : public KeyFrame<Core::Transform>
{
  public:
    /// CONSTRUCTOR
    explicit KeyTransform( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Transform>( time ) {}
    KeyTransform( const KeyTransform& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyTransform() override {}

  protected:
    /// TRANSFORMATION
    inline Core::Transform defaultFrame() const override { return Core::Transform::Identity(); }

    inline Core::Transform interpolate( const Core::Transform& F0,
                                        const Core::Transform& F1,
                                        const Scalar t ) const override {
        Core::Transform result;
        Core::Animation::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_TRANSFORM_HPP
