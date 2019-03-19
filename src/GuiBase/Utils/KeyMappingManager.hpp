#ifndef RADIUMENGINE_KEYMAPPINGMANAGER_HPP
#define RADIUMENGINE_KEYMAPPINGMANAGER_HPP

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtXml/QtXml>

#include <GuiBase/RaGuiBase.hpp>

#include <Core/Utils/Singleton.hpp>

namespace Ra {
namespace Gui {
/**
 * An utility class used to map a (combination) of key / modifier to a specific action.
 * It can load configuration from a file or if no config is found it will load an
 * internal version of the default configuration.
 */
class RA_GUIBASE_API KeyMappingManager {
    RA_SINGLETON_INTERFACE( KeyMappingManager );

    // Needed by Q_ENUM
    Q_GADGET

  public:
    /**
     * Definition of the KeyMappingAction identifier.
     * Represents all of the actions which can be done with a combination of
     * key/modifier or with a mouse click.
     * If you want to add a new action, just add a value to this list.
     * Try to keep it clear, FILENAME_ACTION_NAME is used here.
     */
#define KeyMappingActionEnumValues             \
    KMA_VALUE( TRACKBALLCAMERA_MANIPULATION )  \
    KMA_VALUE( TRACKBALLCAMERA_ROTATE_AROUND ) \
    KMA_VALUE( GIZMOMANAGER_MANIPULATION )     \
    KMA_VALUE( GIZMOMANAGER_STEP )             \
    KMA_VALUE( VIEWER_BUTTON_PICKING_QUERY )   \
    KMA_VALUE( FEATUREPICKING_VERTEX )         \
    KMA_VALUE( FEATUREPICKING_EDGE )           \
    KMA_VALUE( FEATUREPICKING_TRIANGLE )       \
    KMA_VALUE( FEATUREPICKING_MULTI_CIRCLE )   \
    KMA_VALUE( VIEWER_BUTTON_CAST_RAY_QUERY )  \
    KMA_VALUE( VIEWER_RAYCAST_QUERY )          \
    KMA_VALUE( VIEWER_TOGGLE_WIREFRAME )       \
    KMA_VALUE( COLORWIDGET_PRESSBUTTON )

    /**
     * Enum which represents all of the actions which can be done through
     * keyboard shortcuts.
     */
    enum KeyMappingAction {
#define KMA_VALUE( x ) x,
        KeyMappingActionEnumValues
#undef KMA_VALUE
            KEYMAPPING_ACTION_NUMBER
    };

    /**
     * Static array of string representation of enumeration values.
     */
    static const std::string KeyMappingActionNames[];

    // In order to have a String corresponding to each enum value.
    Q_ENUM( KeyMappingAction )

  public:
    /**
     * Load the mapping configuration from the given file.
     */
    void loadConfiguration( const char* filename = nullptr );

    /**
     * Reload the configuration.
     */
    void reloadConfiguration();

    /**
     * Return the key associated to the given action.
     * \note May be useful in some cases, but prefer actionTriggered()
     *       instead everytime it is possible.
     */
    int getKeyFromAction( KeyMappingAction action );

    /**
     * Return if a QMouseEvent triggers a specific action, false otherwise.
     */

    bool actionTriggered( QMouseEvent* event, KeyMappingAction action );

    /**
     * Return if a QKeyEvent triggers a specific action, false otherwise.
     */
    bool actionTriggered( QKeyEvent* event, KeyMappingAction action );

    /**
     * Stream insertion operator.
     */
    friend std::ostream& operator<<( std::ostream& out,
                                     KeyMappingManager::KeyMappingAction action ) {
        out << KeyMappingManager::KeyMappingActionNames[static_cast<unsigned int>( action )];
        return out;
    }

  private:
    KeyMappingManager();

    ~KeyMappingManager();

    /**
     * Bind the given key to the given action.
     */
    // Private for now, but may need to be public if we want to customize
    // keymapping configuration otherwise than by editing the XML configuration file.
    void bindKeyToAction( int keyCode, KeyMappingAction action );

    /**
     * Internal process to load all the bindings.
     */
    void loadConfigurationInternal();

    /**
     * Internal process to load the binding from a given xml element.
     */
    void loadConfigurationTagsInternal( QDomElement& node );

    /**
     * Internal process to register the binding from the key and action data.
     */
    void loadConfigurationMappingInternal( const std::string& typeString,
                                           const std::string& modifierString,
                                           const std::string& keyString,
                                           const std::string& actionString );

    /**
     * Convert a string to a Qt::KeyboardModifier.
     * \return Qt::NoModifier if the string doesn't correspond to a valid Modifier.
     */
    // Maybe there's a better way to get enum value from string, even without Q_ENUM
    // defined for Qt::KeyboardModifier and Qt::MouseButton ?
    Qt::KeyboardModifier getQtModifierValue( const std::string& modifierString );

    /**
     * Convert a string to a Qt::MouseButton.
     * \return Qt::NoButton if the string doesn't correspond to a valid Button.
     */
    Qt::MouseButton getQtMouseButtonValue( const std::string& keyString );

  private:
    // For XML parsing using Qt
    QDomDocument m_domDocument; ///< The content of the xml file.
    QMetaEnum m_metaEnumAction; ///< The current action enum.
    QMetaEnum m_metaEnumKey;    ///< The current key enum.
    QFile* m_file;              ///< The configuration file.

    /// The list of action to key bindings.
    // Stores enum value as key, if we want to have two actions bound to the same
    // combination (is it possible ?)
    std::map<KeyMappingAction, int> m_mapping;
};
} // namespace Gui
} // namespace Ra

#endif // RADIUMENGINE_KEYMAPPINGMANAGER_HPP
