#ifndef RADIUMENGINE_KEYMAPPINGMANAGER_HPP
#define RADIUMENGINE_KEYMAPPINGMANAGER_HPP

#include <QtXml/QtXml>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Singleton.hpp>

namespace Ra
{
    namespace Gui
    {
        class RA_ENGINE_API KeyMappingManager
        {
            RA_SINGLETON_INTERFACE(KeyMappingManager);

            Q_GADGET

            public:
                enum KeyMappingAction
                {
                    TRACKBALL_CAMERA_MANIPULATION = 0,
                    TRACKBALL_CAMERA_ROTATE_AROUND,

                    VIEWER_TOGGLE_WIREFRAME,
                    VIEWER_LEFT_BUTTON_PICKING_QUERY,
                    VIEWER_RIGHT_BUTTON_PICKING_QUERY,
                    VIEWER_RAYCAST_QUERY,

                    GIZMO_MANIPULATION,

                    COLORWIDGET_PRESSBUTTON
                };

                Q_ENUM(KeyMappingAction)
                Q_ENUM(Qt::KeyboardModifier)

            public:
                void bindKeyToAction( int keyCode, KeyMappingAction action );
                void loadConfiguration( const char * filename = nullptr );
                void reloadConfiguration();

                int getKeyFromAction( KeyMappingAction action );

                bool actionTriggered( QMouseEvent * event, KeyMappingAction action );
                bool actionTriggered( QKeyEvent * event, KeyMappingAction action );

            private:
                KeyMappingManager();
                ~KeyMappingManager();

                void loadConfigurationInternal();
                void loadConfigurationTagsInternal( QDomElement& node );
                void loadConfigurationMappingInternal( const std::string& typeString, const std::string& modifierString,
                                                       const std::string& keyString, const std::string& actionString );

                Qt::KeyboardModifier getQtModifierValue( const std::string& modifierString );
                Qt::MouseButton getQtMouseButtonValue( const std::string& keyString );

            private:
                QDomDocument m_domDocument;
                QMetaEnum m_metaEnumAction;
                QMetaEnum m_metaEnumKey;
                QFile * m_file;

                std::map<KeyMappingAction, int> m_mapping;
        };
    }
}

#endif //RADIUMENGINE_KEYMAPPINGMANAGER_HPP
