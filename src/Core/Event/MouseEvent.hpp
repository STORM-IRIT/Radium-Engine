#ifndef RADIUMENGINE_MOUSEEVENT_HPP
#define RADIUMENGINE_MOUSEEVENT_HPP

#include <Core/RaCore.hpp>
#include <Core/Event/EventEnums.hpp>

namespace Ra
{
    namespace Core
    {

        struct MouseEvent
        {
            /// MouseEventType: Press, Release, Move, Wheel
            int event;
            /// MouseButton : Left button, Right button, Middle button
            int button;

            /// Modifier has been used ? Ctrl, Alt, Shift
            int modifier;

            /// X mouse position in [width, height] when the event occured.
            int absoluteXPosition;
            /// Y mouse position in [width, height] when the event occured.
            int absoluteYPosition;

            /// Wheel delta. Is only set for WheelEvent, undefined otherwise.
            int wheelDelta;
        };

    }
}

#endif // RADIUMENGINE_MOUSEEVENT_HPP
