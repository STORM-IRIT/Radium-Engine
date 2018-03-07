#ifndef RADIUMENGINE_EVENTENUMS_HPP
#define RADIUMENGINE_EVENTENUMS_HPP

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

namespace Modifier {
enum Modifier { RA_EMPTY = 0x1, RA_CTRL_KEY = 0x2, RA_SHIFT_KEY = 0x4, RA_ALT_KEY = 0x8 };
} // namespace Modifier

namespace KeyEventType {
enum KeyEventType { RA_KEY_PRESSED = 0x1, RA_KEY_RELEASED = 0x2 };
} // namespace KeyEventType

namespace MouseEventType {
enum MouseEventType {
    RA_MOUSE_PRESSED = 0x1,
    RA_MOUSE_RELEASED = 0x2,
    RA_MOUSE_MOVED = 0x4,
    RA_MOUSE_WHEEL = 0x8
};
}

namespace MouseButton {
enum MouseButton {
    RA_MOUSE_LEFT_BUTTON = 0x1,
    RA_MOUSE_MIDDLE_BUTTON = 0x2,
    RA_MOUSE_RIGHT_BUTTON = 0x4
};
}
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_EVENTENUMS_HPP
