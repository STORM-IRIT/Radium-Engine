#ifndef RADIUMENGINE_KEY_TRANSLATION_HPP
#define RADIUMENGINE_KEY_TRANSLATION_HPP

#include <Core/Animation/Interpolation.hpp>
#include <Core/Animation/KeyFrame.hpp>

namespace Ra {
namespace Core {
namespace Animation {

class KeyTranslation : public KeyFrame<Core::Vector3> {
  public:
    /// CONSTRUCTOR
    KeyTranslation( const AnimationTime& time = AnimationTime() ) :
        KeyFrame<Core::Vector3>( time ) {}
    KeyTranslation( const KeyTranslation& keyframe ) = default;

    /// DESTRUCTOR
    ~KeyTranslation() {}

  protected:
    /// TRANSFORMATION
    inline Core::Vector3 defaultFrame() const override { return Core::Vector3( 0.0, 0.0, 0.0 ); }

    inline Core::Vector3 interpolate( const Core::Vector3& F0, const Core::Vector3& F1,
                                      const Scalar t ) const override {
        Core::Vector3 result;
        Core::Animation::interpolate( F0, F1, t, result );
        return result;
    }
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEY_TRANSLATION_HPP
