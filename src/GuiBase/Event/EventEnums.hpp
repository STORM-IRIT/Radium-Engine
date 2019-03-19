#ifndef RADIUMENGINE_EVENTENUMS_HPP
#define RADIUMENGINE_EVENTENUMS_HPP

namespace Ra {
namespace GuiBase {

namespace Modifier {
/**
 * Used for indicating which kind of Modifier is applied to an event.
 */
enum Modifier { RA_EMPTY = 0x1, RA_CTRL_KEY = 0x2, RA_SHIFT_KEY = 0x4, RA_ALT_KEY = 0x8 };
} // namespace Modifier

namespace KeyEventType {
/**
 * Used to indicate the type of a KeyEvent.
 */
enum KeyEventType { RA_KEY_PRESSED = 0x1, RA_KEY_RELEASED = 0x2 };
} // namespace KeyEventType

namespace MouseEventType {
/**
 * Used to indicate the type of a MouseEvent.
 */
enum MouseEventType {
    RA_MOUSE_PRESSED = 0x1,
    RA_MOUSE_RELEASED = 0x2,
    RA_MOUSE_MOVED = 0x4,
    RA_MOUSE_WHEEL = 0x8
};
} // namespace MouseEventType

namespace MouseButton {
/**
 * Used to indicate the type of button linked to a MouseEvent (only for pressed and released).
 */
enum MouseButton {
    RA_MOUSE_LEFT_BUTTON = 0x1,
    RA_MOUSE_MIDDLE_BUTTON = 0x2,
    RA_MOUSE_RIGHT_BUTTON = 0x4
};
} // namespace MouseButton
} // namespace GuiBase
} // namespace Ra

#endif // RADIUMENGINE_EVENTENUMS_HPP
