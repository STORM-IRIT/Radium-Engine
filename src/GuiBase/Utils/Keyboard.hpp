#ifndef RADIUMENGINE_KEYBOARD_HPP
#define RADIUMENGINE_KEYBOARD_HPP

#include <GuiBase/RaGuiBase.hpp>

namespace Ra {
namespace Gui {
RA_GUIBASE_API bool isKeyPressed( int code ); ///< is the key corresponding to \param code currently pressed
RA_GUIBASE_API int activeKey(); ///< return the last, still pressed, key code, -1 if no key currently pressed
RA_GUIBASE_API void keyPressed( int code );  ///< set the key corresponding to \param code as pressed
RA_GUIBASE_API void keyReleased( int code ); ///< set the key corresponding to \param code as released
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYBOARD_HPP
