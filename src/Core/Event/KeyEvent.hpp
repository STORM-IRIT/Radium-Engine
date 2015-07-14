#ifndef RADIUMENGINE_KEYEVENT_HPP
#define RADIUMENGINE_KEYEVENT_HPP

#include <Core/Event/EventEnums.hpp>

namespace Ra
{

struct KeyEvent
{
    /// KeyEventType : Press, Release
    int event;
    /// Keycode. Test this with Qt::Keys enum values.
    int key;

    /// Modifier used : Ctrl, Alt, Shift.
    int modifier;
};

} // namespace Ra

#endif // RADIUMENGINE_KEYEVENT_HPP
