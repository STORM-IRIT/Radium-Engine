#include <Gui/Utils/Keyboard.hpp>
#include <map>
#include <utility>

namespace Ra {
namespace Gui {
std::map<int, bool> g_keypresses;

// store the last key pressed, still pressed.
int g_activeKey = -1;

void keyPressed( int code ) {
    g_keypresses[code] = true;
    g_activeKey        = code;
}

void keyReleased( int code ) {
    g_keypresses[code] = false;
    if ( code == g_activeKey ) g_activeKey = -1;
}

bool isKeyPressed( int code ) {
    // Default constructed bool is false, so this should be enough
    return g_keypresses[code];
}

int activeKey() {
    return g_activeKey;
}

void releaseAllKeys() {
    for ( auto& k : g_keypresses ) {
        k.second = false;
    }
    g_activeKey = -1;
}

} // namespace Gui
} // namespace Ra
