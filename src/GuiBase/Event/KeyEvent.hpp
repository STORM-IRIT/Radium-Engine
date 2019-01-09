#ifndef RADIUMENGINE_KEYEVENT_HPP
#define RADIUMENGINE_KEYEVENT_HPP

#include <GuiBase/Event/EventEnums.hpp>

namespace Ra {
namespace GuiBase {

struct KeyEvent {
    /// KeyEventType : Press, Release
    KeyEventType::KeyEventType event;
    /// Keycode. Test this with Qt::Keys enum values.
    int key;

    /// Modifier used : Ctrl, Alt, Shift.
    Modifier::Modifier modifier;
};

} // namespace GuiBase
} // namespace Ra

#endif // RADIUMENGINE_KEYEVENT_HPP
