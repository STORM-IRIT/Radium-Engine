#ifndef RADIUMENGINE_MOUSEEVENT_HPP
#define RADIUMENGINE_MOUSEEVENT_HPP

#include <GuiBase/Event/EventEnums.hpp>

namespace Ra {
namespace GuiBase {

struct MouseEvent {
    /// MouseEventType: Press, Release, Move, Wheel
    MouseEventType::MouseEventType event;
    /// MouseButton : Left button, Right button, Middle button
    MouseButton::MouseButton button;

    /// Modifier has been used ? Ctrl, Alt, Shift
    Modifier::Modifier modifier;

    /// X mouse position in [width, height] when the event occured.
    int absoluteXPosition;
    /// Y mouse position in [width, height] when the event occured.
    int absoluteYPosition;

    /// Wheel delta. Is only set for WheelEvent, undefined otherwise.
    int wheelDelta;
};

} // namespace GuiBase
} // namespace Ra

#endif // RADIUMENGINE_MOUSEEVENT_HPP
