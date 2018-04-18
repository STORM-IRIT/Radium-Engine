#ifndef RADIUMENGINE_RENDEROBJECTTYPES_HPP
#define RADIUMENGINE_RENDEROBJECTTYPES_HPP

namespace Ra {
namespace Engine {

enum class RenderObjectType {
    /// "Ui" render objects are drawn on top of everything else and view independant
    UI = 0,
    /// "Debug" render objects are drawn like any other object (not lit though)
    Debug,
    /// "Fancy" render objects are the ones that should be lit and so on
    Fancy,

    Count
};
}
} // namespace Ra
#endif // RADIUMENGINE_RENDEROBJECT_TYPES
