#include <Gui/Utils/KeyMappingManager.hpp>
namespace Ra {
namespace Gui {

inline bool KeyMappingManager::EventBinding::operator<( const EventBinding& b ) const {
    return ( m_buttons < b.m_buttons ) ||
           ( ( m_buttons == b.m_buttons ) && ( m_modifiers < b.m_modifiers ) ) ||
           ( ( m_buttons == b.m_buttons ) && ( m_modifiers == b.m_modifiers ) &&
             ( m_key < b.m_key ) ) ||
           ( ( m_buttons == b.m_buttons ) && ( m_modifiers == b.m_modifiers ) &&
             ( m_key == b.m_key ) && ( m_wheel < b.m_wheel ) );
}
} // namespace Gui
} // namespace Ra
