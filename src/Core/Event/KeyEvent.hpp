#ifndef RADIUMENGINE_KEYEVENT_HPP
#define RADIUMENGINE_KEYEVENT_HPP

#include <Core/Event/EventEnums.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

/// Stores the data related to a key event.
struct KeyEvent {
    /// Keycode. Test this with Qt::Keys enum values.
    int key;

    /// KeyEventType : Press, Release
    KeyEventType::KeyEventType event;

    /// Modifier used : Ctrl, Alt, Shift.
    Modifier::Modifier modifier;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_KEYEVENT_HPP
