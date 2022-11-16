#pragma once

#include <QXmlStreamWriter>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtXml/QtXml>

#include <Gui/RaGui.hpp>

#include <Core/Utils/Index.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/Observable.hpp>
#include <Core/Utils/Singleton.hpp>
#include <Core/Utils/StdOptional.hpp>

namespace Ra {
namespace Gui {
/// An utility class used to map a (combination) of key / modifier to a specific action.
/// It can load configuration from a file or if no config is found it will load an
/// internal version of the default configuration.
class RA_GUI_API KeyMappingManager : public Ra::Core::Utils::ObservableVoid
{
    RA_SINGLETON_INTERFACE( KeyMappingManager );

  public:
    /// Inner class to store event binding.
    class EventBinding
    {
      public:
        /**
         * Empty event ctor
         */
        EventBinding() = default;

        /**
         * Specifies all data members.
         * \note key should not contains modifier, use \b modifiers instead. This is not checked.
         */
        EventBinding( Qt::MouseButtons buttons,
                      Qt::KeyboardModifiers modifiers,
                      int key,
                      bool wheel = false ) :
            m_buttons { buttons }, m_modifiers { modifiers }, m_key { key }, m_wheel { wheel } {}

        /**
         *  Mouse event ctor.
         */
        explicit EventBinding( Qt::MouseButtons buttons,
                               Qt::KeyboardModifiers modifiers = Qt::NoModifier ) :
            m_buttons { buttons }, m_modifiers { modifiers } {}

        /**
         * Key event ctor.
         */
        explicit EventBinding( int key, Qt::KeyboardModifiers modifiers = Qt::NoModifier ) :
            m_modifiers { modifiers }, m_key { key } {}

        /**
         * Wheel event ctor.
         */
        explicit EventBinding( bool wheel, Qt::KeyboardModifiers modifiers = Qt::NoModifier ) :
            m_modifiers { modifiers }, m_wheel { wheel } {}

        bool operator<( const EventBinding& b ) const;
        bool isMouseEvent() { return m_buttons != Qt::NoButton; }
        bool isWheelEvent() { return m_wheel; }
        bool isKeyEvent() { return !isMouseEvent() && !isWheelEvent(); }

        // public data member, since there isn't any write access getter from KeyMappingManager
        Qt::MouseButtons m_buttons { Qt::NoButton };
        Qt::KeyboardModifiers m_modifiers { Qt::NoModifier };
        // only one key
        int m_key { -1 };
        bool m_wheel { false };
    };

    using KeyMappingAction = Ra::Core::Utils::Index;
    using Context          = Ra::Core::Utils::Index;

    /// load configuration from filename, or default configration filename. It
    /// calls listeners callbacks then.
    void loadConfiguration( const std::string& filename = {} );

    /// Save the configuration
    /// @param filename the file to write to. It will be replaced
    /// @return true if file was correctly saved
    bool saveConfiguration( const std::string& filename = {} );

    /// reload last open file.
    void reloadConfiguration();

    std::string getLoadedFilename() { return m_file->fileName().toStdString(); }

    /// Return the action associated to the binding buttons + modifiers + key
    /// \param context is the context to get action from (e.g. camera or gizmo)
    /// \param buttons are the mouse buttons pressed, could be NoButton
    /// \param modifiers are the keyboard modifiers, could be NoModifier
    /// \param key is the key pressed, could be -1
    /// \param wheel specifies if we consider a wheel event
    KeyMappingAction getAction( const Context& context,
                                const Qt::MouseButtons& buttons,
                                const Qt::KeyboardModifiers& modifiers,
                                int key,
                                bool wheel = false );
    KeyMappingAction
    getAction( const Context& context, const QEvent* event, int key, bool wheel = false );
    KeyMappingAction getAction( const Context& context, const EventBinding& binding );

    /// Return, if exists, the event binding associated with a context/action.
    /// if such binding doesn't exists, the optional does not contain a value.
    std::optional<EventBinding> getBinding( const Context& context, KeyMappingAction action );

    /// Add an action from its name. If the actionName was already present, the privously added
    /// action is returned.
    KeyMappingAction addAction( const Context& context, const std::string& actionName );

    /// Add an action from its name, with the given binding.  If the actionName was already present,
    /// the privously added action is returned.
    KeyMappingAction
    addAction( const Context& context, const EventBinding& binding, const std::string& actionName );

    /// Bind binding to action, in context. It replace previously
    /// binded action, with a warning if binding was alreasly present.
    void setActionBinding( const Context& context,
                           const EventBinding& binding,
                           const KeyMappingAction& action );

    /// \brief Creates the context index for the given context name.
    ///
    /// If the context already exist, return the existing index. If not, the context is created
    /// and its index is returned.
    /// \param contextName the name of the context
    /// \return a valid context index for the given context name.
    Context addContext( const std::string& contextName );

    /// Return the context index corresponding to contextName
    /// \param contextName the name of the context
    /// \return an invalid context if contextName has not been created
    Context getContext( const std::string& contextName );

    /// Return the action index corresponding to a context index and actionName
    /// \param context the index of the context
    /// \param actionName the name of the action
    /// \return an invalid action if context is not valid, or if actionName has  not been created.
    /// (i,e action.isInvalid())
    KeyMappingAction getAction( const Context& context, const std::string& actionName );

    /// \return Action name if context index and action index are valid, "Invalid" otherwise
    std::string getActionName( const Context& context, const KeyMappingAction& action );

    /// \return Context name if context index is valid, "Invalid" otherwise
    std::string getContextName( const Context& context );

    /// Add a callback, triggered when configuration is load or reloaded.
    int addListener( Observable::Observer callback );

    /// Remove a callback. To be called when the related Context/Actions are no more needed.
    /// @param callbackId the Id, returned by addListener, of the Observer to be removed.
    void removeListener( int callbackId );

    /// Return a string corresponding to the current dom document.
    std::string getHelpText();

    /// return a string of enum names from mouse buttons, comma separated,
    /// without space.
    static std::string enumNamesFromMouseButtons( const Qt::MouseButtons& buttons );

    /// return a string of enum names from keyboard modifiers, comma separated,
    /// without space.
    static std::string enumNamesFromKeyboardModifiers( const Qt::KeyboardModifiers& modifiers );

    /**
     * EventBinding factory from string description of buttons, modifiers, key and wheel.
     */
    static EventBinding createEventBindingFromStrings( const std::string& buttonsString   = "",
                                                       const std::string& modifiersString = "",
                                                       const std::string& keyString       = "",
                                                       const std::string& wheelString     = "" );

  private:
    KeyMappingManager();
    ~KeyMappingManager();

    /// Save an XML node that describes an event/action.
    void saveNode( QXmlStreamWriter& stream, const QDomNode& domNode );
    void saveKeymap( QXmlStreamWriter& stream );

    void loadConfigurationInternal( const QDomDocument& domDocument );
    void loadConfigurationTagsInternal( QDomElement& node );
    KeyMappingAction loadConfigurationMappingInternal( const std::string& context,
                                                       const std::string& keyString,
                                                       const std::string& modifiersString,
                                                       const std::string& buttonsString,
                                                       const std::string& wheelString,
                                                       const std::string& actionName );

    /// Return KeyboardModifiers described in modifierString, multiple modifiers
    /// are comma separated in the modifiers string, as in
    /// "ShiftModifier,AltModifier". This function do note trim any white space.
    static Qt::KeyboardModifiers getQtModifiersValue( const std::string& modifierString );

    /// Return MouseButtons desribed in buttonString, multiple modifiers
    /// are comma separated in the modifiers string, \note only one button is
    /// supported for the moment.
    static Qt::MouseButtons getQtMouseButtonsValue( const std::string& buttonsString );
    static int getKeyCode( const std::string& keyString );

    std::string m_defaultConfigFile;
    QFile* m_file;

    using EventBindingMap = std::map<EventBinding, KeyMappingAction>;
    using ContextNameMap  = std::map<std::string, Context>;
    using ActionNameMap   = std::map<std::string, Ra::Core::Utils::Index>;

    ContextNameMap m_contextNameToIndex;            ///< context string give index
    std::vector<ActionNameMap> m_actionNameToIndex; ///< one element per context
    std::vector<EventBindingMap> m_bindingToAction; ///< one element per context
};

/// KeyMappingManageable decorate, typical use as a CRTP :
/// class MyClass : public KeyMappingManageable<MyClass> { [...]
/// it defines a static class member m_keyMappingContext, readable with
/// getContext()
/// This context index must be registered by the class to the KeyMappingManager
/// with KeyMappingManager::getContext("MyClassContextName"); after a
/// configuration file is loaded.
template <typename T>
class KeyMappingManageable
{
  public:
    static inline KeyMappingManager::Context getContext() { return m_keyMappingContext; }

    /// KeyManageable class should implement static configureKeyMapping_impl to get their
    /// keyMappingAction constants.
    static inline void configureKeyMapping() { T::configureKeyMapping_impl(); }

  protected:
    T& self() { return static_cast<T&>( *this ); }
    static inline void setContext( const KeyMappingManager::Context& c ) {
        m_keyMappingContext = c;
    }

  private:
    static KeyMappingManager::Context m_keyMappingContext;
};

/// create one m_keyMappingContext by template type.
template <typename T>
KeyMappingManager::Context KeyMappingManageable<T>::m_keyMappingContext;

class RA_GUI_API KeyMappingCallbackManager
{
  public:
    using Context          = KeyMappingManager::Context;
    using KeyMappingAction = KeyMappingManager::KeyMappingAction;
    using Callback         = std::function<void( QEvent* )>;

    inline explicit KeyMappingCallbackManager( Context context );

    inline void addEventCallback( KeyMappingAction action, Callback callback );

    inline KeyMappingManager::KeyMappingAction
    addActionAndCallback( const std::string& actionName,
                          const KeyMappingManager::EventBinding& binding,
                          Callback callback );

    inline bool triggerEventCallback( KeyMappingAction actionIndex, QEvent* event );
    inline bool triggerEventCallback( QEvent* event, int key, bool wheel = false );

  private:
    std::map<KeyMappingAction, Callback> m_keymappingCallbacks;
    Context m_context;
};

} // namespace Gui
} // namespace Ra

#include <Gui/Utils/KeyMappingManager.inl>
