#ifndef RADIUMENGINE_KEYMAPPINGMANAGER_HPP
#define RADIUMENGINE_KEYMAPPINGMANAGER_HPP

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtXml/QtXml>

#include <GuiBase/RaGuiBase.hpp>

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
    /// Definition of the KeyMappingAction identifier
    /// Represents all of the actions which can be done with a combination of key/modifier or with a
    /// mouse click. If you want to add a new action, just add a value to this list. Try to keep it
    /// clear, FILENAME_ACTION_NAME is used here
#define KeyMappingActionEnumValues             \
    KMA_VALUE( TRACKBALLCAMERA_MANIPULATION )  \
    KMA_VALUE( TRACKBALLCAMERA_ROTATE )        \
    KMA_VALUE( TRACKBALLCAMERA_PAN )           \
    KMA_VALUE( TRACKBALLCAMERA_ZOOM )          \
    KMA_VALUE( TRACKBALLCAMERA_ROTATE_AROUND ) \
    KMA_VALUE( GIZMOMANAGER_MANIPULATION )     \
    KMA_VALUE( GIZMOMANAGER_STEP )             \
    KMA_VALUE( VIEWER_PICKING )                \
    KMA_VALUE( VIEWER_PICKING_VERTEX )         \
    KMA_VALUE( VIEWER_PICKING_EDGE )           \
    KMA_VALUE( VIEWER_PICKING_TRIANGLE )       \
    KMA_VALUE( VIEWER_PICKING_MULTI_CIRCLE )   \
    KMA_VALUE( VIEWER_BUTTON_CAST_RAY_QUERY )  \
    KMA_VALUE( VIEWER_RAYCAST )                \
    KMA_VALUE( VIEWER_TOGGLE_WIREFRAME )       \
    KMA_VALUE( COLORWIDGET_PRESSBUTTON )

    /// Enum which represents all of the actions which can be done
    enum KeyMappingAction {
#define KMA_VALUE( x ) x,
        KeyMappingActionEnumValues
#undef KMA_VALUE
            KEYMAPPING_ACTION_NUMBER
    };

    /// Static array of string representation of enumeration values.
    static const std::string KeyMappingActionNames[];

    // In order to have a String corresponding to each enum value
    Q_ENUM( KeyMappingAction )

  public:
    void loadConfiguration( const char* filename = nullptr );
    void reloadConfiguration();

    /// Return the action associated to the binding buttons + modifiers + key
    /// \param buttons are the mouse buttons pressed, could be NoButton
    /// \param modifiers are the keyboard modifiers, could be NoModifiers
    /// \param key is the key pressed, could be -1
    KeyMappingAction
    getAction( Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, int key );

    friend std::ostream& operator<<( std::ostream& out,
                                     KeyMappingManager::KeyMappingAction action ) {
        out << KeyMappingManager::KeyMappingActionNames[static_cast<unsigned int>( action )];
        return out;
    }

    static std::string enumNamesFromMouseButtons( const Qt::MouseButtons& buttons );
    static std::string enumNamesFromKeyboardModifiers( const Qt::KeyboardModifiers& modifiers );

  private:
    KeyMappingManager();
    ~KeyMappingManager();

    // Private for now, but may need to be public if we want to customize keymapping configuration
    // otherwise than by editing the XML configuration file.
    void bindKeyToAction( int keyCode,
                          Qt::KeyboardModifiers,
                          Qt::MouseButtons,
                          KeyMappingAction action );

    void loadConfigurationInternal();
    void loadConfigurationTagsInternal( QDomElement& node );
    void loadConfigurationMappingInternal( const std::string& typeString,
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
   MouseBindingMapping m_mappingAction;
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYMAPPINGMANAGER_HPP
