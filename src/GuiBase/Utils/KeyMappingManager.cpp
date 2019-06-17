#include "KeyMappingManager.hpp"

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Gui {

using namespace Core::Utils; // log

const std::string KeyMappingManager::KeyMappingActionNames[] = {
#define KMA_VALUE( x ) std::string( #x ),
    KeyMappingActionEnumValues
#undef KMA_VALUE
        std::string( "InvalidKeyMapping" )};

KeyMappingManager::KeyMappingManager() :
    m_domDocument( "Keymapping QDomDocument" ),
    m_metaEnumAction( QMetaEnum::fromType<KeyMappingAction>() ),
    m_metaEnumKey( QMetaEnum::fromType<Qt::Key>() ),
    m_file( nullptr ) {
    QSettings settings;
    QString keymappingfilename =
        settings.value( "keymapping/config", "Configs/default.xml" ).toString();
    if ( !keymappingfilename.contains( "default.xml" ) )
    {
        LOG( logINFO ) << "Loading keymapping " << keymappingfilename.toStdString() << " (from "
                       << settings.fileName().toStdString() << ")";
    }
    loadConfiguration( keymappingfilename.toStdString().c_str() );
}

KeyMappingManager::KeyMappingAction
KeyMappingManager::getAction( Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, int key ) {
    MouseBinding binding{buttons, modifiers, key};

    auto action = m_mappingAction.find( binding );

    if ( action != m_mappingAction.end() ) { return action->second; }
    return KEYMAPPING_ACTION_NUMBER;
}

void KeyMappingManager::bindKeyToAction( int keyCode,
                                         Qt::KeyboardModifiers modifiers,
                                         Qt::MouseButtons buttons,
                                         KeyMappingAction action ) {
    MouseBinding binding{buttons, modifiers, keyCode};
    auto f = m_mappingAction.find( binding );
    if ( f != m_mappingAction.end() )
    {
        LOG( logWARNING ) << "Binding action " << action << " to "
                          << "buttons [" << enumNamesFromMouseButtons( buttons ) << "] "
                          << "modifiers [" << enumNamesFromKeyboardModifiers( modifiers ) << "] "
                          << "keycode [" << keyCode << "]"
                          << ", which is already used for action " << f->second << ".";
    }
    m_mappingAction[binding] = action;
}

void KeyMappingManager::loadConfiguration( const char* filename ) {
    // if no filename is given, load default configuration
    if ( !filename ) { filename = "Configs/default.xml"; }

    delete m_file;
    m_file = new QFile( filename );

    if ( !m_file->open( QIODevice::ReadOnly ) )
    {
        if ( strcmp( filename, "Configs/default.xml" ) != 0 )
        {
            LOG( logERROR ) << "Failed to open keymapping configuration file ! "
                            << m_file->fileName().toStdString();
            LOG( logERROR ) << "Trying to load default configuration...";
            loadConfiguration();
            return;
        }
        else
        {
            LOG( logERROR ) << "Failed to open default keymapping configuration file !";
            return;
        }
    }

    if ( !m_domDocument.setContent( m_file ) )
    {
        LOG( logERROR ) << "Can't associate XML file to QDomDocument !";
        LOG( logERROR ) << "Trying to load default configuration...";

        loadConfiguration();
        return;
    }

    // Store setting only if not default
    if ( !strcmp( filename, "Configs/default.xml" ) )
    {
        QSettings settings;
        settings.setValue( "keymapping/config", m_file->fileName() );
    }
    m_file->close();

    loadConfigurationInternal();
}

void KeyMappingManager::loadConfigurationInternal() {
    m_mappingAction.clear();

    QDomElement domElement = m_domDocument.documentElement();
    QDomNode node          = domElement.firstChild();

    if ( domElement.tagName() != "keymaps" )
    {
        LOG( logWARNING ) << "No <keymaps> global bounding tag ! Maybe you set a different global "
                             "tag ? (Not a big deal)";
    }

    while ( !node.isNull() )
    {
        QDomElement nodeElement = node.toElement();
        loadConfigurationTagsInternal( nodeElement );
        node = node.nextSibling();
    }
}

void KeyMappingManager::loadConfigurationTagsInternal( QDomElement& node ) {
    if ( node.tagName() == "keymap" )
    {

        QDomElement e = node.toElement();
        std::string keyString = e.attribute( "key" ).toStdString();
        std::string modifiersString =
            node.toElement().attribute( "modifiers", "NoModifier" ).toStdString();
        std::string buttonsString =
            node.toElement().attribute( "buttons", "NoButton" ).toStdString();
        std::string actionString = node.toElement().attribute( "action" ).toStdString();
        loadConfigurationMappingInternal( keyString, modifiersString, buttonsString, actionString );
    }
    else
    {
        LOG( logERROR ) << "Unrecognized XML keymapping configuration file tag \""
                        << qPrintable( node.tagName() ) << "\" !";
        LOG( logERROR ) << "Trying to load default configuration...";
        loadConfiguration();

        return;
    }
}

void KeyMappingManager::loadConfigurationMappingInternal( const std::string& keyString,
                                                          const std::string& modifiersString,
                                                          const std::string& buttonsString,
                                                          const std::string& actionString ) {

    auto actionValue                     = m_metaEnumAction.keyToValue( actionString.c_str() );
    Qt::KeyboardModifiers modifiersValue = getQtModifiersValue( modifiersString );
    auto keyValue                        = m_metaEnumKey.keyToValue( keyString.c_str() );
    auto buttonsValue                    = getQtMouseButtonsValue( buttonsString );

    if ( actionValue == -1 )
    {
        LOG( logERROR ) << "No valid action [" << actionString << "] specified for binding key ["
                        << keyString << "], and buttons[" << buttonsString << "]";
    }
    else if ( keyValue == -1 && buttonsValue == Qt::NoButton )
    {
        LOG( logERROR ) << "No key nor mouse buttons specified for action [" << actionString
                        << "] with key [" << keyString << "], and buttons[" << buttonsString << "]";
        LOG( logERROR ) << "Trying to load default configuration...";
    }
    else
    {
        bindKeyToAction(
            keyValue, modifiersValue, buttonsValue, static_cast<KeyMappingAction>( actionValue ) );
    }
}

Qt::KeyboardModifiers KeyMappingManager::getQtModifiersValue( const std::string& modifierString ) {
    Qt::KeyboardModifiers modifier = Qt::NoModifier;

    if ( modifierString == "ShiftModifier" ) { modifier = Qt::ShiftModifier; }
    else if ( modifierString == "ControlModifier" )
    { modifier = Qt::ControlModifier; }
    else if ( modifierString == "AltModifier" )
    { modifier = Qt::AltModifier; }
    else if ( modifierString == "MetaModifier" )
    { modifier = Qt::MetaModifier; }
    else if ( modifierString == "KeypadModifier" )
    { modifier = Qt::KeypadModifier; }
    else if ( modifierString == "GroupSwitchModifier" )
    { modifier = Qt::GroupSwitchModifier; }

    return modifier;
}

Qt::MouseButtons KeyMappingManager::getQtMouseButtonsValue( const std::string& keyString ) {
    Qt::MouseButtons key = Qt::NoButton;

    if ( keyString == "LeftButton" ) { key = Qt::LeftButton; }
    else if ( keyString == "RightButton" )
    { key = Qt::RightButton; }
    else if ( keyString == "MidButton" )
    { key = Qt::MidButton; }
    else if ( keyString == "MiddleButton" )
    { key = Qt::MiddleButton; }
    else if ( keyString == "XButton1" )
    { key = Qt::XButton1; }
    else if ( keyString == "XButton2" )
    { key = Qt::XButton2; }

    return key;
}

void KeyMappingManager::reloadConfiguration() {
    if ( !m_file->isOpen() ) { return; }

    QString filename = m_file->fileName();
    m_file->close();
    loadConfiguration( filename.toStdString().c_str() );
}

KeyMappingManager::~KeyMappingManager() {
    if ( m_file->isOpen() ) { m_file->close(); }
}

std::string KeyMappingManager::enumNamesFromMouseButtons( const Qt::MouseButtons& buttons ) {
    std::string returnText;
    if ( buttons == Qt::NoButton ) return "NoButton";
    if ( buttons & Qt::LeftButton ) returnText += "LeftButton ";
    if ( buttons & Qt::RightButton ) returnText += "RightButton ";
    if ( buttons & Qt::MiddleButton ) returnText += "MiddleButton ";
    if ( buttons & Qt::BackButton ) returnText += "BackButton ";
    if ( buttons & Qt::ForwardButton ) returnText += "ForwardButton ";
    if ( buttons & Qt::TaskButton ) returnText += "TaskButton ";
    if ( buttons & Qt::ExtraButton4 ) returnText += "ExtraButton4 ";
    if ( buttons & Qt::ExtraButton5 ) returnText += "ExtraButton5 ";
    if ( buttons & Qt::ExtraButton6 ) returnText += "ExtraButton6 ";
    if ( buttons & Qt::ExtraButton7 ) returnText += "ExtraButton7 ";
    if ( buttons & Qt::ExtraButton8 ) returnText += "ExtraButton8 ";
    if ( buttons & Qt::ExtraButton9 ) returnText += "ExtraButton9 ";
    if ( buttons & Qt::ExtraButton10 ) returnText += "ExtraButton10 ";
    if ( buttons & Qt::ExtraButton11 ) returnText += "ExtraButton11 ";
    if ( buttons & Qt::ExtraButton12 ) returnText += "ExtraButton12 ";
    if ( buttons & Qt::ExtraButton13 ) returnText += "ExtraButton13 ";
    if ( buttons & Qt::ExtraButton14 ) returnText += "ExtraButton14 ";
    if ( buttons & Qt::ExtraButton15 ) returnText += "ExtraButton15 ";
    if ( buttons & Qt::ExtraButton16 ) returnText += "ExtraButton16 ";
    if ( buttons & Qt::ExtraButton17 ) returnText += "ExtraButton17 ";
    if ( buttons & Qt::ExtraButton18 ) returnText += "ExtraButton18 ";
    if ( buttons & Qt::ExtraButton19 ) returnText += "ExtraButton19 ";
    if ( buttons & Qt::ExtraButton20 ) returnText += "ExtraButton20 ";
    if ( buttons & Qt::ExtraButton21 ) returnText += "ExtraButton21 ";
    if ( buttons & Qt::ExtraButton22 ) returnText += "ExtraButton22 ";
    if ( buttons & Qt::ExtraButton23 ) returnText += "ExtraButton23 ";
    if ( buttons & Qt::ExtraButton24 ) returnText += "ExtraButton24 ";
    return returnText;
}

std::string
KeyMappingManager::enumNamesFromKeyboardModifiers( const Qt::KeyboardModifiers& buttons ) {
    std::string returnText;
    if ( buttons == Qt::NoModifier ) return "NoModifier";
    if ( buttons & Qt::ShiftModifier ) returnText += "ShiftModifier ";
    if ( buttons & Qt::ControlModifier ) returnText += "ControlModifier ";
    if ( buttons & Qt::AltModifier ) returnText += "AltModifier ";
    if ( buttons & Qt::MetaModifier ) returnText += "MetaModifier ";
    if ( buttons & Qt::KeypadModifier ) returnText += "KeypadModifier ";
    if ( buttons & Qt::GroupSwitchModifier ) returnText += "GroupSwitchModifier ";

    return returnText;
}

RA_SINGLETON_IMPLEMENTATION( KeyMappingManager );
} // namespace Gui
} // namespace Ra
