#ifndef RADIUMENGINE_MOUSEEVENT_HPP
#define RADIUMENGINE_MOUSEEVENT_HPP

#include <GuiBase/Event/EventEnums.hpp>

namespace Ra {
namespace GuiBase {

/**
 * Stores the data related to a mouse event.
 */
struct MouseEvent {
    /// MouseEventType: Press, Release, Move or Wheel.
    MouseEventType::MouseEventType event;

    /// MouseButton: Left button, Right button or Middle button.
    MouseButton::MouseButton button;

    /// Modifier used: Ctrl, Alt or Shift.
    Modifier::Modifier modifier;

    /// X mouse position in [width, height] when the event occured.
    int absoluteXPosition;

    /// Y mouse position in [width, height] when the event occured.
    int absoluteYPosition;

    /// Wheel delta. It is only set for WheelEvents, undefined otherwise.
    int wheelDelta;
};

} // namespace GuiBase
} // namespace Ra

#endif // RADIUMENGINE_MOUSEEVENT_HPP
