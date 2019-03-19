#ifndef RADIUMENGINE_KEYBOARD_HPP
#define RADIUMENGINE_KEYBOARD_HPP

#include <GuiBase/RaGuiBase.hpp>

namespace Ra {
namespace Gui {

/**
 * Return true if the given key has been pressed and not yet released, false otherwise.
 */
RA_GUIBASE_API bool isKeyPressed( int code );

/**
 * Notify the given key has been pressed.
 */
RA_GUIBASE_API void keyPressed( int code );

/**
 * Notify the given key has been released.
 */
RA_GUIBASE_API void keyReleased( int code );
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYBOARD_HPP
