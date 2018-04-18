#include <GuiBase/Utils/Keyboard.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/Log/Log.hpp>

#include <map>

namespace Ra {
namespace Gui {
std::map<int, bool> g_keypresses;

void keyPressed( int code ) {
    g_keypresses[code] = true;
}

void keyReleased( int code ) {
    g_keypresses[code] = false;
}

bool isKeyPressed( int code ) {
    // Default constructed bool is false, so this should be enough
    return g_keypresses[code];
}
} // namespace Gui
} // namespace Ra
