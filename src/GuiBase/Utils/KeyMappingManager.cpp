#include "KeyMappingManager.hpp"

#include <Core/Resources/Resources.hpp>
#include <Core/Utils/Log.hpp>

namespace Ra::Gui {

using namespace Core::Utils; // log

KeyMappingManager::KeyMappingManager() :
    m_defaultConfigFile( std::string( Core::Resources::getBaseDir() ) +
                         std::string( "Configs/default.xml" ) ),
    m_domDocument( "Key Mapping QDomDocument" ),
    m_metaEnumKey( QMetaEnum::fromType<Qt::Key>() ),
    m_file( nullptr ) {
    QSettings settings;
    QString keyMappingFilename =
        settings.value( "keymapping/config", m_defaultConfigFile.c_str() ).toString();
    if ( !keyMappingFilename.contains( "default.xml" ) )
    {
        LOG( logINFO ) << "Loading key mapping " << keyMappingFilename.toStdString() << " (from "
                       << settings.fileName().toStdString() << ")";
    }
    else
    { LOG( logINFO ) << "Loading default key mapping " << m_defaultConfigFile; }
    loadConfiguration( keyMappingFilename.toStdString().c_str() );
}

KeyMappingManager::KeyMappingAction
KeyMappingManager::getAction( const KeyMappingManager::Context& context,
                              const Qt::MouseButtons& buttons,
                              const Qt::KeyboardModifiers& modifiers,
                              int key,
                              bool wheel ) {

    if ( context.isInvalid() )
    {
        LOG( logINFO ) << "try to get action from an invalid context";
        return KeyMappingAction();
    }
    // skip key as modifiers,
    if ( ( key == Qt::Key_Shift ) || ( key == Qt::Key_Control ) || ( key == Qt::Key_Alt ) ||
         ( key == Qt::Key_Meta ) )
    { key = -1; }
    KeyMappingManager::MouseBinding binding{buttons, modifiers, key, wheel};

    auto action = m_mappingAction[context].find( binding );
    if ( action != m_mappingAction[context].end() ) { return action->second; }

    return KeyMappingManager::KeyMappingAction();
}

KeyMappingManager::Context KeyMappingManager::getContext( const std::string& contextName ) {
    // use find so that it do not insert invalid context
    auto itr = m_contextNameToIndex.find( contextName );
    if ( itr != m_contextNameToIndex.end() ) return itr->second;
    return Context{};
}

KeyMappingManager::KeyMappingAction
KeyMappingManager::getActionIndex( const Context& context, const std::string& actionName ) {
    if ( context >= m_actionNameToIndex.size() || context.isInvalid() )
    {
        LOG( logINFO ) << "try to get action index from an invalid context";

        return KeyMappingAction{};
    }
    auto itr = m_actionNameToIndex[context].find( actionName );
    if ( itr != m_actionNameToIndex[context].end() ) return itr->second;
    LOG( logINFO ) << "try to get action index from an invalid action name " << actionName
                   << "(context #" << context << ")";

    return KeyMappingAction{};
}

std::string KeyMappingManager::getActionName( const Context& context,
                                              const KeyMappingAction& action ) {

    if ( context < m_actionNameToIndex.size() && context.isValid() )
    {

        auto actionFindItr = std::find_if(
            std::begin( m_actionNameToIndex[context] ),
            std::end( m_actionNameToIndex[context] ),
            [&]( const ActionNameMap::value_type& pair ) { return pair.second == action; } );

        if ( actionFindItr != std::end( m_actionNameToIndex[context] ) )
        { return actionFindItr->first; }
    }
    return "Invalid";
}

std::string KeyMappingManager::getContextName( const Context& context ) {
    auto contextFindItr = std::find_if(
        std::begin( m_contextNameToIndex ),
        std::end( m_contextNameToIndex ),
        [&]( const ContextNameMap ::value_type& pair ) { return pair.second == context; } );

    if ( contextFindItr != std::end( m_contextNameToIndex ) ) { return contextFindItr->first; }
    return "Invalid";
}

void KeyMappingManager::addListener( Observer callback ) {
    attach( callback );
    // call the registered listener directly to have it up to date if the
    // config is already loaded
    callback();
}

void KeyMappingManager::bindKeyToAction( Ra::Core::Utils::Index contextIndex,
                                         const MouseBinding& binding,
                                         Ra::Core::Utils::Index actionIndex ) {

    CORE_ASSERT( contextIndex < m_contextNameToIndex.size(), "contextIndex is out of range" );

    // search if an action already correspond to this binding.
    auto f = m_mappingAction[contextIndex].find( binding );
    if ( f != m_mappingAction[contextIndex].end() )
    {

        // if yes, search for its name
        auto findResult = std::find_if(
            std::begin( m_actionNameToIndex[contextIndex] ),
            std::end( m_actionNameToIndex[contextIndex] ),
            [&]( const ActionNameMap::value_type& pair ) { return pair.second == actionIndex; } );

        // if the name is not present, something bad happens.
        if ( findResult == std::end( m_actionNameToIndex[contextIndex] ) )
        {
            LOG( logERROR ) << "Corrupted call to bindKeyToAction, index " << actionIndex
                            << " must have been inserted before !\n";
            return;
        }

        // if name is present, find the other action's name
        auto findResult2 = std::find_if(
            std::begin( m_actionNameToIndex[contextIndex] ),
            std::end( m_actionNameToIndex[contextIndex] ),
            [&]( const ActionNameMap::value_type& pair ) { return pair.second == f->second; } );

        // if the name is not present, something bad happens.
        if ( findResult2 == std::end( m_actionNameToIndex[contextIndex] ) )
        {
            LOG( logERROR ) << "Corrupted call to bindKeyToAction, index " << actionIndex
                            << " must have been inserted before !\n";
            return;
        }

        LOG( logWARNING ) << "Binding action " << findResult->first << " to "
                          << "buttons [" << enumNamesFromMouseButtons( binding.m_buttons ) << "] "
                          << "modifiers [" << enumNamesFromKeyboardModifiers( binding.m_modifiers )
                          << "]"
                          << " keycode [" << binding.m_key << "]"
                          << " wheel [" << binding.m_wheel << "]"
                          << ", which is already used for action " << findResult2->first << ".";
    }

    LOG( logDEBUG4 ) << "In context " << getContextName( contextIndex ) << " [" << contextIndex
                     << "]"
                     << " binding action " << getActionName( contextIndex, actionIndex ) << " ["
                     << actionIndex << "]"
                     << " buttons [" << enumNamesFromMouseButtons( binding.m_buttons ) << "]"
                     << " modifiers [" << enumNamesFromKeyboardModifiers( binding.m_modifiers )
                     << "]"
                     << " keycode [" << binding.m_key << "]"
                     << " wheel [" << binding.m_wheel << "]";

    m_mappingAction[contextIndex][binding] = actionIndex;
}

void KeyMappingManager::loadConfiguration( const char* filename ) {
    // if no filename is given, load default configuration
    if ( !filename ) { filename = m_defaultConfigFile.c_str(); }

    delete m_file;
    m_file = new QFile( filename );

    if ( !m_file->open( QIODevice::ReadOnly ) )
    {
        if ( strcmp( filename, m_defaultConfigFile.c_str() ) != 0 )
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
        m_file->close();
        loadConfiguration();
        return;
    }

    // Store setting only if not default
    if ( !strcmp( filename, m_defaultConfigFile.c_str() ) )
    {
        QSettings settings;
        settings.setValue( "keymapping/config", m_file->fileName() );
    }
    m_file->close();

    loadConfigurationInternal();

    // notify observer that keymapping has changed.
    notify();
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

    std::istringstream f( modifierString );
    std::string s;

    while ( getline( f, s, ',' ) )
    {
        if ( s == "ShiftModifier" ) { modifier |= Qt::ShiftModifier; }
        else if ( s == "ControlModifier" )
        { modifier |= Qt::ControlModifier; }
        else if ( s == "AltModifier" )
        { modifier |= Qt::AltModifier; }
        else if ( s == "MetaModifier" )
        { modifier |= Qt::MetaModifier; }
        else if ( s == "KeypadModifier" )
        { modifier |= Qt::KeypadModifier; }
        else if ( s == "GroupSwitchModifier" )
        { modifier |= Qt::GroupSwitchModifier; }
    }

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
    ///\todo add other buttons support here
    return key;
}

void KeyMappingManager::reloadConfiguration() {
    QString filename = m_file->fileName();
    loadConfiguration( filename.toStdString().c_str() );
}

KeyMappingManager::~KeyMappingManager() {
    if ( m_file->isOpen() ) { m_file->close(); }
}

#define TEST_BUTTON_STRING( BUTTON ) \
    if ( buttons & Qt::BUTTON )      \
    {                                \
        returnText += sep + #BUTTON; \
        sep = ",";                   \
    }

std::string KeyMappingManager::enumNamesFromMouseButtons( const Qt::MouseButtons& buttons ) {
    std::string returnText;
    std::string sep;

    if ( buttons == Qt::NoButton ) return "NoButton";

    TEST_BUTTON_STRING( LeftButton );
    TEST_BUTTON_STRING( RightButton );
    TEST_BUTTON_STRING( MiddleButton );
    TEST_BUTTON_STRING( BackButton );
    TEST_BUTTON_STRING( ForwardButton );
    TEST_BUTTON_STRING( TaskButton );
    TEST_BUTTON_STRING( ExtraButton4 );
    TEST_BUTTON_STRING( ExtraButton5 );
    TEST_BUTTON_STRING( ExtraButton6 );
    TEST_BUTTON_STRING( ExtraButton7 );
    TEST_BUTTON_STRING( ExtraButton8 );
    TEST_BUTTON_STRING( ExtraButton9 );
    TEST_BUTTON_STRING( ExtraButton10 );
    TEST_BUTTON_STRING( ExtraButton11 );
    TEST_BUTTON_STRING( ExtraButton12 );
    TEST_BUTTON_STRING( ExtraButton13 );
    TEST_BUTTON_STRING( ExtraButton14 );
    TEST_BUTTON_STRING( ExtraButton15 );
    TEST_BUTTON_STRING( ExtraButton16 );
    TEST_BUTTON_STRING( ExtraButton17 );
    TEST_BUTTON_STRING( ExtraButton18 );
    TEST_BUTTON_STRING( ExtraButton19 );
    TEST_BUTTON_STRING( ExtraButton20 );
    TEST_BUTTON_STRING( ExtraButton21 );
    TEST_BUTTON_STRING( ExtraButton22 );
    TEST_BUTTON_STRING( ExtraButton23 );
    TEST_BUTTON_STRING( ExtraButton24 );

    return returnText;
}

std::string
KeyMappingManager::enumNamesFromKeyboardModifiers( const Qt::KeyboardModifiers& buttons ) {
    std::string returnText;
    std::string sep;

    if ( buttons == Qt::NoModifier ) return "NoModifier";

    TEST_BUTTON_STRING( ShiftModifier );
    TEST_BUTTON_STRING( ControlModifier );
    TEST_BUTTON_STRING( AltModifier );
    TEST_BUTTON_STRING( MetaModifier );
    TEST_BUTTON_STRING( KeypadModifier );
    TEST_BUTTON_STRING( GroupSwitchModifier );

    return returnText;
}

#undef TEST_BUTTON_STRING

RA_SINGLETON_IMPLEMENTATION( KeyMappingManager );
} // namespace Ra::Gui
