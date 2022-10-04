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

inline KeyMappingCallbackManager::KeyMappingCallbackManager( Context context ) :
    m_context { context } {}

inline void KeyMappingCallbackManager::addEventCallback( KeyMappingAction action,
                                                         Callback callback ) {
    m_keymappingCallbacks[action] = callback;
}

inline KeyMappingManager::KeyMappingAction
KeyMappingCallbackManager::addActionAndCallback( const std::string& actionName,
                                                 const KeyMappingManager::EventBinding& binding,
                                                 Callback callback ) {

    auto mgr    = KeyMappingManager::getInstance();
    auto action = mgr->addAction( m_context, binding, actionName );
    addEventCallback( action, callback );
    return action;
}

inline bool KeyMappingCallbackManager::triggerEventCallback( KeyMappingAction actionIndex,
                                                             QEvent* event ) {
    auto itr = m_keymappingCallbacks.find( actionIndex );
    if ( itr == m_keymappingCallbacks.end() ) return false;
    itr->second( event );
    return true;
}

inline bool KeyMappingCallbackManager::triggerEventCallback( QEvent* event, int key, bool wheel ) {
    auto mgr         = KeyMappingManager::getInstance();
    auto actionIndex = mgr->getAction( m_context, event, key, wheel );
    return triggerEventCallback( actionIndex, event );
}

} // namespace Gui
} // namespace Ra
