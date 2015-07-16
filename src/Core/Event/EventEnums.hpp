#ifndef RADIUMENGINE_EVENTENUMS_HPP
#define RADIUMENGINE_EVENTENUMS_HPP

namespace Ra { namespace Core
{

namespace Modifier
{
    enum Modifier
    {
        CTRL_KEY = 0x1,
        SHIFT_KEY = 0x2,
        ALT_KEY = 0x4
    };
} // namespace Modifier

namespace KeyEventType
{
    enum KeyEventType
    {
        KEY_PRESSED = 0x1,
        KEY_RELEASED = 0x2
    };
} // namespace KeyEventType

namespace MouseEventType
{
    enum MouseEventType
    {
        MOUSE_PRESSED = 0x1,
        MOUSE_RELEASED = 0x2,
        MOUSE_MOVED = 0x4,
        MOUSE_WHEEL = 0x8
    };
}

namespace MouseButton
{
    enum MouseButton
    {
        MOUSE_LEFT_BUTTON = 0x1,
        MOUSE_MIDDLE_BUTTON = 0x2,
        MOUSE_RIGHT_BUTTON = 0x4
    };
}

}} // namespace Ra::Core

#endif // RADIUMENGINE_EVENTENUMS_HPP
