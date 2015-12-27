#ifndef RADIUMENGINE_RENDEROBJECTTYPES_HPP
#define RADIUMENGINE_RENDEROBJECTTYPES_HPP

namespace Ra
{
namespace Engine
{

enum class RenderObjectType
{
    /// "Ui" render objects are drawn on top of everything else and view independant
    UI = 0,
    /// "Debug xray" render objects with no depth test
    DEBUG_XRAY,
    /// "Debug other" render objects are drawn like any other object (not lit though)
    DEBUG_OTHER,

    /// "Fancy" render objects are the ones that should be lit and so on
    FANCY,

    COUNT
};

}
}
#endif // RADIUMENGINE_RENDEROBJECT_TYPES
