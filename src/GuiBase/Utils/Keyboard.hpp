#ifndef RADIUMENGINE_KEYBOARD_HPP
#define RADIUMENGINE_KEYBOARD_HPP

namespace Ra {
namespace Gui {

/// Return true if the corresponding key is pressed.
bool isKeyPressed( int code );

/// Notify that the corresponding key is pressed.
void keyPressed( int code );

/// Notify that the corresponding key is released.
void keyReleased( int code );

} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYBOARD_HPP
