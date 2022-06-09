#pragma once

#include <Gui/RaGui.hpp>

namespace Ra {
namespace Gui {
RA_GUI_API bool
isKeyPressed( int code ); ///< is the key corresponding to \param code currently pressed
RA_GUI_API int
activeKey(); ///< return the last, still pressed, key code, -1 if no key currently pressed
RA_GUI_API void keyPressed( int code );  ///< set the key corresponding to \param code as pressed
RA_GUI_API void keyReleased( int code ); ///< set the key corresponding to \param code as released
RA_GUI_API void releaseAllKeys();        ///< set all key's status to released (e.g. for focusOut
                                         /// event). and activeKey to -1.
} // namespace Gui
} // namespace Ra
