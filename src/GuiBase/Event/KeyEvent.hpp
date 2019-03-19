#ifndef RADIUMENGINE_KEYEVENT_HPP
#define RADIUMENGINE_KEYEVENT_HPP

#include <GuiBase/Event/EventEnums.hpp>

namespace Ra {
namespace GuiBase {

/**
 * Stores the data related to a key event.
 */
struct KeyEvent {
    /// KeyEventType: Press or Release.
    KeyEventType::KeyEventType event;

    /// Keycode. Test this with Qt::Keys enum values.
    int key;

    /// Modifier used: <Ctrl>, <Alt> or <Shift>.
    Modifier::Modifier modifier;
};

} // namespace GuiBase
} // namespace Ra

#endif // RADIUMENGINE_KEYEVENT_HPP
