#ifndef RADIUMENGINE_MOUSEEVENT_HPP
#define RADIUMENGINE_MOUSEEVENT_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Event/EventEnums.hpp>

namespace Ra { namespace Core
{

struct MouseEvent
{
    /// MouseEventType: Press, Release, Move, Wheel
    int event;
    /// MouseButton : Left button, Right button, Middle button
    int button;

    /// Modifier has been used ? Ctrl, Alt, Shift
    int modifier;

    /// X mouse position in [0,1] when the event occured.
    Scalar relativeXPosition;
    /// Y mouse position in [0,1] when the event occured.
    Scalar relativeYPosition;

    /// X mouse position in [width, height] when the event occured.
    Scalar absoluteXPosition;
    /// Y mouse position in [width, height] when the event occured.
    Scalar absoluteYPosition;

    /// Wheel delta. Is only set for WheelEvent, undefined otherwise.
    Scalar wheelDelta;
};

}}

#endif // RADIUMENGINE_MOUSEEVENT_HPP
