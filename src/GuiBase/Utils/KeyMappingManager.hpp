#ifndef RADIUMENGINE_KEYMAPPINGMANAGER_HPP
#define RADIUMENGINE_KEYMAPPINGMANAGER_HPP

#include <QtXml/QtXml>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include <GuiBase/RaGuiBase.hpp>

#include <Core/Utils/Singleton.hpp>

namespace Ra
{
    namespace Gui
    {
        /// An utility class used to map a (combination) of key / modifier to a specific action.
        /// It can load configuration from a file or if no config is found it will load an
        /// internal version of the default configuration.
        class RA_GUIBASE_API KeyMappingManager
        {
            RA_SINGLETON_INTERFACE(KeyMappingManager);

            // Needed by Q_ENUM
            Q_GADGET

            public:
                /// An enum which represents all of the actions which can be done
                /// with a combination of key/modifier or with a mouse click.
                /// If you want to add a new action, just add an value to this enum.
                /// Try to keep it clear, FILENAME_ACTION_NAME is used here (maybe not the best way to do it ?)
                enum KeyMappingAction
                {
                    TRACKBALLCAMERA_MANIPULATION = 0,
                    TRACKBALLCAMERA_ROTATE_AROUND,

                    GIZMOMANAGER_MANIPULATION,

                    VIEWER_BUTTON_PICKING_QUERY,
                    FEATUREPICKING_VERTEX,
                    FEATUREPICKING_EDGE,
                    FEATUREPICKING_TRIANGLE,
                    FEATUREPICKING_MULTI_CIRCLE,

                    VIEWER_BUTTON_CAST_RAY_QUERY,
                    VIEWER_RAYCAST_QUERY,

                    VIEWER_TOGGLE_WIREFRAME,

                    COLORWIDGET_PRESSBUTTON
                };

                // In order to have a String corresponding to each enum value
                Q_ENUM(KeyMappingAction)

            public:
                void loadConfiguration( const char * filename = nullptr );
                void reloadConfiguration();

                /// May be useful in some cases, but try to use actionTriggered
                /// instead everytime it is possible.
                int getKeyFromAction( KeyMappingAction action );

                /// Utility method to check if a QMouseEvent triggers a specific action
                bool actionTriggered( QMouseEvent * event, KeyMappingAction action );
                /// Utility method to check if a QKeyEvent triggers a specific action
                bool actionTriggered( QKeyEvent * event, KeyMappingAction action );

            private:
                KeyMappingManager();
                ~KeyMappingManager();

                // Private for now, but may need to be public if we want to customize keymapping configuration
                // otherwise than by editing the XML configuration file.
                void bindKeyToAction( int keyCode, KeyMappingAction action );

                void loadConfigurationInternal();
                void loadConfigurationTagsInternal( QDomElement& node );
                void loadConfigurationMappingInternal( const std::string& typeString, const std::string& modifierString,
                                                       const std::string& keyString, const std::string& actionString );

                // Maybe there's a better way to get enum value from string, even without Q_ENUM
                // defined for Qt::KeyboardModifier and Qt::MouseButton ?
                Qt::KeyboardModifier getQtModifierValue( const std::string& modifierString );
                Qt::MouseButton getQtMouseButtonValue( const std::string& keyString );

            private:
                // For XML parsing using Qt
                QDomDocument m_domDocument;
                QMetaEnum m_metaEnumAction;
                QMetaEnum m_metaEnumKey;
                QFile * m_file;

                // Stores enum value as key, if we want to have two actions bound to the same
                // combination (is it possible ?)
                std::map<KeyMappingAction, int> m_mapping;
        };
    }
}

#endif //RADIUMENGINE_KEYMAPPINGMANAGER_HPP
