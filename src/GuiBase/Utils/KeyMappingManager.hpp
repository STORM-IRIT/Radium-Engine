#ifndef RADIUMENGINE_KEYMAPPINGMANAGER_HPP
#define RADIUMENGINE_KEYMAPPINGMANAGER_HPP

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtXml/QtXml>

#include <GuiBase/RaGuiBase.hpp>

#include <Core/Utils/Index.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/Singleton.hpp>

namespace Ra {
namespace Gui {
/// An utility class used to map a (combination) of key / modifier to a specific action.
/// It can load configuration from a file or if no config is found it will load an
/// internal version of the default configuration.
class RA_GUIBASE_API KeyMappingManager
{
    RA_SINGLETON_INTERFACE( KeyMappingManager );

    // Needed by Q_ENUM
    Q_GADGET

  public:
    using Listener = void ( * )();
    using KeyMappingAction = Ra::Core::Utils::Index;
    using Context          = Ra::Core::Utils::Index;

    void loadConfiguration( const char* filename = nullptr );
    void reloadConfiguration();

    /// Return the action associated to the binding buttons + modifiers + key
    /// \param buttons are the mouse buttons pressed, could be NoButton
    /// \param modifiers are the keyboard modifiers, could be NoModifiers
    /// \param key is the key pressed, could be -1
    KeyMappingAction getAction( const Context& context,
                                const Qt::MouseButtons& buttons,
                                const Qt::KeyboardModifiers& modifiers,
                                int key );

    static std::string enumNamesFromMouseButtons( const Qt::MouseButtons& buttons );
    static std::string enumNamesFromKeyboardModifiers( const Qt::KeyboardModifiers& modifiers );

    Context getContext( const std::string contextName ) {
        return m_contextNameToIndex[contextName];
    }
    KeyMappingAction getActionIndex( const Context& context, const std::string actionName ) {
        LOG( Ra::Core::Utils::logINFO ) << "getActionIndex " << context << " " << actionName;

        return m_actionNameToIndex[context][actionName];
    }

    std::string getActionName( const Context& context, const KeyMappingAction& action ) {
        auto findResult = std::find_if(
            std::begin( m_actionNameToIndex[context] ),
            std::end( m_actionNameToIndex[context] ),
            [&]( const ActionNameMap::value_type& pair ) { return pair.second == action; } );

        if ( findResult != std::end( m_actionNameToIndex[context] ) ) { return findResult->first; }
        return "Invalid";
    }

    std::string getContextName( const Context& context ) {
        auto findResult = std::find_if(
            std::begin( m_contextNameToIndex ),
            std::end( m_contextNameToIndex ),
            [&]( const ContextNameMap ::value_type& pair ) { return pair.second == context; } );

        if ( findResult != std::end( m_contextNameToIndex ) ) { return findResult->first; }
        return "Invalid";
    }

    /// Add a callback, triggered when configuration is load or reloaded.
    void addListener( Listener callback ) {
        m_listeners.push_back( callback );
        // call the registered listener directly to have it up to date.
        callback();
    }

  private:
    KeyMappingManager();
    ~KeyMappingManager();

    // Private for now, but may need to be public if we want to customize keymapping configuration
    // otherwise than by editing the XML configuration file.
    void bindKeyToAction( Ra::Core::Utils::Index contextIndex,
                          int keyCode,
                          Qt::KeyboardModifiers,
                          Qt::MouseButtons,
                          Ra::Core::Utils::Index actionIndex );

    void loadConfigurationInternal();
    void loadConfigurationTagsInternal( QDomElement& node );
    void loadConfigurationMappingInternal( const std::string& context,
                                           const std::string& typeString,
                                           const std::string& modifierString,
                                           const std::string& keyString,
                                           const std::string& actionString );

    // Maybe there's a better way to get enum value from string, even without Q_ENUM
    // defined for Qt::KeyboardModifier and Qt::MouseButton ?
    Qt::KeyboardModifiers getQtModifiersValue( const std::string& modifierString );
    Qt::MouseButtons getQtMouseButtonsValue( const std::string& keyString );

  private:
    // For XML parsing using Qt
    QDomDocument m_domDocument;
    QMetaEnum m_metaEnumAction;
    QMetaEnum m_metaEnumKey;
    QFile* m_file;

    class MouseBinding
    {
      public:
        explicit MouseBinding( Qt::MouseButtons buttons,
                               Qt::KeyboardModifiers modifiers = Qt::NoModifier,
                               int key                         = -1 ) :

            m_buttons{buttons},
            m_modifiers{modifiers},
            m_key{key} {}
        bool operator<( const MouseBinding& b ) const {
            return ( m_buttons < b.m_buttons ) ||
                   ( ( m_buttons == b.m_buttons ) && ( m_modifiers < b.m_modifiers ) ) ||
                   ( ( m_buttons == b.m_buttons ) && ( m_modifiers == b.m_modifiers ) &&
                     ( m_key < b.m_key ) );
        }

      private:
        Qt::MouseButtons m_buttons;
        Qt::KeyboardModifiers m_modifiers;
        // only one key
        int m_key;
    };

    using MouseBindingMapping = std::map<MouseBinding, KeyMappingAction>;
    using ContextNameMap      = std::map<std::string, Context>;
    using ActionNameMap       = std::map<std::string, Ra::Core::Utils::Index>;

    std::vector<Listener> m_listeners;

    ContextNameMap m_contextNameToIndex;              ///< context string give index
    std::vector<ActionNameMap> m_actionNameToIndex;   ///< one element per context
    std::vector<MouseBindingMapping> m_mappingAction; ///< one element per context
};

template <typename T>
class KeyMappingManageable
{
  public:
    //    static virtual void registerKeyMapping() = 0;

    /*
        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleMouseReleaseEvent( QMouseEvent* event ) { return false; }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleMouseMoveEvent( QMouseEvent* event,
                                           const Qt::MouseButtons& buttons,
                                           const Qt::KeyboardModifiers& modifiers,
                                           int key ) {
            return false;
        }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleWheelEvent( QWheelEvent* event ) { return false; }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleKeyPressEvent( QKeyEvent* event,
                                          const KeyMappingManager::KeyMappingAction& action ) {
            return false;
        }

        /// @return true if the event has been taken into account, false otherwise
        virtual bool handleKeyReleaseEvent( QKeyEvent* event ) { return false; }
    */
    static KeyMappingManager::Context getContext() { return m_keyMappingContext; }

  protected:
    static KeyMappingManager::Context m_keyMappingContext;
};

template <typename T>
KeyMappingManager::Context KeyMappingManageable<T>::m_keyMappingContext;

}; // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYMAPPINGMANAGER_HPP
