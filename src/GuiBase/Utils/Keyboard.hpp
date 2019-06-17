#ifndef RADIUMENGINE_KEYBOARD_HPP
#define RADIUMENGINE_KEYBOARD_HPP

namespace Ra {
namespace Gui {
bool isKeyPressed( int code ); /// is the key corresponding to \param code currently pressed
int activeKey(); /// return the last, still pressed, key code, -1 if no key currently pressed
void keyPressed( int code );  /// set the key corresponding to \param code as pressed
void keyReleased( int code ); /// set the key corresponding to \param code as released
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYBOARD_HPP
