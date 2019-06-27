#include "KeyMappingManager.hpp"

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Gui {

using namespace Core::Utils; // log

KeyMappingManager::KeyMappingManager() :
    m_domDocument( "Key Mapping QDomDocument" ),
    m_metaEnumKey( QMetaEnum::fromType<Qt::Key>() ),
    m_file( nullptr ) {
    QSettings settings;
    QString keyMappingFilename =
        settings.value( "keymapping/config", "Configs/default.xml" ).toString();
    if ( !keyMappingFilename.contains( "default.xml" ) )
    {
        LOG( logINFO ) << "Loading key mapping " << keyMappingFilename.toStdString() << " (from "
                       << settings.fileName().toStdString() << ")";
    }
    loadConfiguration( keyMappingFilename.toStdString().c_str() );
}

KeyMappingManager::KeyMappingAction
KeyMappingManager::getAction( const KeyMappingManager::Context& context,
                              const Qt::MouseButtons& buttons,
                              const Qt::KeyboardModifiers& modifiers,
                              int key,
                              bool wheel ) {
    CORE_ASSERT( context.isValid(), "try to get action from an invalid context" );

    // skip key as modifiers,
    if ( ( key == Qt::Key_Shift ) || ( key == Qt::Key_Control ) || ( key == Qt::Key_Alt ) ||
         ( key == Qt::Key_Meta ) )
    { key = -1; }

    KeyMappingManager::MouseBinding binding{buttons, modifiers, key, wheel};

    auto action = m_mappingAction[context].find( binding );
    if ( action != m_mappingAction[context].end() ) { return action->second; }

    return KeyMappingManager::KeyMappingAction();
}

void KeyMappingManager::bindKeyToAction( Ra::Core::Utils::Index contextIndex,
                                         const MouseBinding& binding,
                                         Ra::Core::Utils::Index actionIndex ) {

    CORE_ASSERT( contextIndex < m_contextNameToIndex.size(), "contextIndex is out of range" );

    auto f = m_mappingAction[contextIndex].find( binding );
    if ( f != m_mappingAction[contextIndex].end() )
    {

        auto findResult = std::find_if(
            std::begin( m_actionNameToIndex[contextIndex] ),
            std::end( m_actionNameToIndex[contextIndex] ),
            [&]( const ActionNameMap::value_type& pair ) { return pair.second == actionIndex; } );
        if ( findResult == std::end( m_actionNameToIndex[contextIndex] ) )
        {
            LOG( logERROR ) << "Corrupted call to bindKeyToAction index " << actionIndex
                            << " must have been "
                               "inserted before !\n";
        }
        auto test = findResult->first;

        auto findResult2 = std::find_if(
            std::begin( m_actionNameToIndex[contextIndex] ),
            std::end( m_actionNameToIndex[contextIndex] ),
            [&]( const ActionNameMap::value_type& pair ) { return f->second == actionIndex; } );

        LOG( logWARNING ) << "Binding action " << findResult->first << " to "
                          << "buttons [" << enumNamesFromMouseButtons( binding.m_buttons ) << "] "
                          << "modifiers [" << enumNamesFromKeyboardModifiers( binding.m_modifiers )
                          << "] "
                          << "keycode [" << binding.m_key << "]"
                          << "wheel [" << binding.m_wheel << "]"
                          << ", which is already used for action " << findResult2->first << ".";
    }

    LOG( logDEBUG4 ) << "In context " << getContextName( contextIndex ) << " [" << contextIndex
                     << "]"
                     << " binding action " << getActionName( contextIndex, actionIndex ) << " ["
                     << actionIndex << "]"
                     << " buttons [" << enumNamesFromMouseButtons( binding.m_buttons ) << "] "
                     << " modifiers [" << enumNamesFromKeyboardModifiers( binding.m_modifiers )
                     << "] "
                     << " keycode [" << binding.m_key << "]"
                     << " wheel [" << binding.m_wheel << "]";

    m_mappingAction[contextIndex][binding] = actionIndex;
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
            LOG( logERROR ) << "Failed to open key mapping configuration file ! "
                            << m_file->fileName().toStdString();
            LOG( logERROR ) << "Trying to load default configuration...";
            loadConfiguration();
            return;
        }
        else
        {
            LOG( logERROR ) << "Failed to open default key mapping configuration file !";
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

    for ( auto l : m_listeners )
    {
        l();
    }
}

void KeyMappingManager::loadConfigurationInternal() {
    ///\todo maybe find a better way to handle laod and reload.
    /// -> do not clear m_contextNameToIndex m_actionNameToIndex so the keep their index values ...
    m_contextNameToIndex.clear();
    m_actionNameToIndex.clear();
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

        QDomElement e         = node.toElement();
        std::string keyString = e.attribute( "key", "-1" ).toStdString();
        std::string modifiersString =
            node.toElement().attribute( "modifiers", "NoModifier" ).toStdString();
        std::string buttonsString =
            node.toElement().attribute( "buttons", "NoButton" ).toStdString();
        std::string contextString =
            node.toElement().attribute( "context", "AppContext" ).toStdString();
        std::string wheelString  = node.toElement().attribute( "wheel", "false" ).toStdString();
        std::string actionString = node.toElement().attribute( "action" ).toStdString();

        loadConfigurationMappingInternal(
            contextString, keyString, modifiersString, buttonsString, wheelString, actionString );
    }
    else
    {
        LOG( logERROR ) << "Unrecognized XML key mapping configuration file tag \""
                        << qPrintable( node.tagName() ) << "\" !";
        LOG( logERROR ) << "Trying to load default configuration...";
        loadConfiguration();

        return;
    }
}

void KeyMappingManager::loadConfigurationMappingInternal( const std::string& context,
                                                          const std::string& keyString,
                                                          const std::string& modifiersString,
                                                          const std::string& buttonsString,
                                                          const std::string& wheelString,
                                                          const std::string& actionString ) {

    Ra::Core::Utils::Index contextIndex;
    auto contextItr = m_contextNameToIndex.find( context );
    if ( contextItr == m_contextNameToIndex.end() )
    {
        contextIndex                  = m_contextNameToIndex.size();
        m_contextNameToIndex[context] = contextIndex;
        m_actionNameToIndex.emplace_back();
        m_mappingAction.emplace_back();

        CORE_ASSERT( m_actionNameToIndex.size() == contextIndex + 1, "Corrupted actionName DB" );
        CORE_ASSERT( m_mappingAction.size() == contextIndex + 1, "Corrupted mappingAction DB" );
    }
    else
        contextIndex = contextItr->second;

    Ra::Core::Utils::Index actionIndex;
    auto actionItr = m_actionNameToIndex[contextIndex].find( actionString );
    if ( actionItr == m_actionNameToIndex[contextIndex].end() )
    {
        actionIndex                                     = m_actionNameToIndex[contextIndex].size();
        m_actionNameToIndex[contextIndex][actionString] = actionIndex;
    }
    else
    {
        LOG( logERROR ) << "Action " << actionString << " has already been inserted to index for "
                        << context;

        actionIndex = actionItr->second;
    }

    Qt::KeyboardModifiers modifiersValue = getQtModifiersValue( modifiersString );
    auto keyValue                        = m_metaEnumKey.keyToValue( keyString.c_str() );
    auto buttonsValue                    = getQtMouseButtonsValue( buttonsString );
    auto wheel                           = wheelString.compare( "true" ) == 0;

    if ( keyValue == -1 && buttonsValue == Qt::NoButton && !wheel )
    {
        LOG( logERROR ) << "No key nor mouse buttons specified for action [" << actionString
                        << "] with key [" << keyString << "], and buttons[" << buttonsString << "]";
        LOG( logERROR ) << "Trying to load default configuration...";
    }
    else
    {
        bindKeyToAction( contextIndex,
                         MouseBinding{buttonsValue, modifiersValue, keyValue, wheel},
                         actionIndex );
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
