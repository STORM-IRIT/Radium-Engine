\page develkeymapping  KeyMapping
[TOC]

## Intoduction

KeyMapping is defined around two concepts, context (KeyMappingManager::Context) and action (KeyMappingManager::KeyMappingAction).
An action is associated to an event binding (KeyMappingManager::EventBinding), this binding represents the input (mouse, keys, modifiers) that are associated with the action.
The context allows to use same binding for different actions depending of the considered context.
The first widget that receive input event needs to manage context and determine which context to use to request corresponding action.
Typically the viewer is this first widget, and can decide which context to use depending on what is under mouse, if gizmo edit is enable, or if an active camera manipulator is active.

Context and action are simply indices stored in the key mapping manager, they are associated with std::string name, to ease key mapping management.
Typical usage is to have one context per class that may have its specifiv key mapping actions.

On the user side, one can test which action corresponds to a given binding (the one that triggers the event for instance), or use KeyMappingCallback manager to trigger registered callback.

## Usage

You first need to add into every configuration file (located in the `install_dir/Configs/` folder) an entry which will bind your action to a key.
This file as an only `keymaps` node, with `keymap` per binding.

~~~{.xml}
<keymaps>
    <keymap context="CONTEXT" key="KEY" modifiers="MODIFIERS" buttons="BUTTONS" wheel="WHEEL" action="ACTION" />
    <!-- as many entry as needed -->
</keymaps>
~~~

Default values are in \[ \]

* `ACTION` *mandatory* represents the KeyMappingAction enum's value you want to trigger.
* `CONTEXT` \[AppContext\] represents the context, for instance the class that will try to catch this keymapping
* `KEY` \[-1\] represents the key that need to be pressed to trigger the event (ie `Key_Z`, for example), "" or "-1" or absent correspond to no key needed.
* `MODIFIERS` \[NoModifiers\] represents the modifier used along with key or mouse button (needs to be a `Qt::Modifier` enum value) to trigger the action.
Multiples modifiers can be specified, separated by commas as in `"ControlModifier,ShiftModifier"`.
* `BUTTONS` \[NoButtons\] represents the button to trigger the event (i.e. `LeftButton`, for example).
* `WHEEL` \[false\] if true, it's a wheel event ! (anything else is false).

If only a key is defined then it's a `keyPressedEvent`.
If buttons is defined, then it's a `mouse[move/press/release]Event`, that optionally take modifiers and key into account.
If wheel is true, then it's a wheel event, that optionally take modifiers, buttons and key into account.

On the implementation side, your class `C` need (could) derive from `KeyMappingManageable<C>`, it defines the static member variable
`m_keyMappingContext`, and the static function `configureKeyMapping()`, that calls `configureKeyMapping_impl()`, which have to be implemented in your class `C`.

Then you need to define your specific actions as static member of your class

~~~{.cpp}
class MyClass :public KeyManageable<MyClass> {
    friend class Ra::Gui::KeyMappingManageable<MyClass>;

// callback when a configuration file is loaded, could check if the context and action are present.
static void configureKeyMapping(){
    Ra::Gui::KeyMappingManageable<MyClass>::setContext( Ra::Gui::KeyMappingManager::getInstance()->getContext( "MyClassContext" ) );
    m_myAction =  Ra::Gui::KeyMappingManager::getInstance()->getActionIndex( "MyActionName" );
}

//[...]
static Ra::Gui::KeyMappingManager::KeyMappingAction m_myAction;
// [...]
};


// typically in .ccp  file
Ra::Gui::KeyMappingManager::KeyMappingAction m_myAction;

// then typically in main baseApplication ctor or Viewer ctor, but after KeyMappingManager instance is created :
Ra::Gui::KeyMappingManager::getInstance()->addListener(MyClass:configureKeyMapping);

~~~

It can be done with a specific macro :

~~~{.cpp}
// in header
#define KeyMappingMyClass \
    KMA_VALUE( MY_ACTION )

#define KMA_VALUE( XX ) static Ra::Gui::KeyMappingManager::KeyMappingAction XX;
    KeyMappingMyClass
#undef KMA_VALUE

// in source
using MyKeyMapping = Ra::Gui::KeyMappingManageable<MyClass>;

#define KMA_VALUE( XX ) Ra::Gui::KeyMappingManager::KeyMappingAction MyClass::XX;
KeyMappingMyClass
#undef KMA_VALUE

void MyClass::configureKeyMapping() {
    MyKeyMapping::setContext( Ra::Gui::KeyMappingManager::getInstance()->getContext( "MyClassContext" ) );
    if ( MyKeyMapping::getContext().isInvalid() )
        LOG( logINFO ) << "MyClassContext not defined (maybe the configuration file does not contain it";
#define KMA_VALUE( XX ) \
    XX = Ra::Gui::KeyMappingManager::getInstance()->getActionIndex( MyKeyMapping::getContext(), #XX );
    KeyMappingMyClass
#undef KMA_VALUE
}

~~~

## Implementation note

The viewer is the main entry point to dispatch key and mouse event.
The idea is that at a key press or mouse press event, the viewer is capable of determining which class will receive the events.

For instance see `Viewer.cpp` `Viewer::mousePressEvent`:
 \snippet Gui/Viewer/Viewer.cpp event dispatch

## Key mapping and inheritence

If you want to define a derived class that inherits a base class with key mapping, and you want to have key mapping management in this derived, you have to consider the following implementation details:

* Base and derived classes will have two different contexts if the derived class also implements KeyMappingManageable, if not derived class can use the base class context.
* Derived  `setupKeymappingcallbacks` should call base  `setupKeymappingcallbacks`, hence derived `configureKeymapping_impl` should only care about its own `KeyMappingAction`.

Here is an example snippets.

~~~{.cpp}
class MyViewer : public Ra::Gui::Viewer, public Ra::Gui::KeyMappingManageable<MyViewer>
{
    using baseKeyMapping = Ra::Gui::KeyMappingManageable<Ra::Gui::Viewer>;
    using thisKeyMapping = Ra::Gui::KeyMappingManageable<MyViewer>;
    void setupKeyMappingCallbacks() override;
    friend class Ra::Gui::KeyMappingManageable<MyViewer>;

//[...]
#define KeyMappingMyViewer \
    KMA_VALUE( MY_ACTION )
//[...]
}

void  MyViewer::setupKeyMappingCallbacks() {
 // Setup keymapping for base
    base::setupKeyMappingCallbacks();

 // then extend
    auto keyMappingManager = Gui::KeyMappingManager::getInstance();
    keyMappingManager->addListener( thisKeyMapping::configureKeyMapping );
}

void MyViewer::configureKeyMapping_impl() {
    auto keyMappingManager              = Gui::KeyMappingManager::getInstance();
    thisKeyMapping::m_keyMappingContext = keyMappingManager->getContext( "MyViewerContext" );
    if ( thisKeyMapping::m_keyMappingContext.isInvalid() )
    {
        LOG( logINFO ) << "MyViewerContext not defined (maybe the configuration file do not contains it)";
        return;
    }

#define KMA_VALUE( XX ) XX = keyMappingManager->getActionIndex( thisKeyMapping::m_keyMappingContext, #XX );

 KeyMappingMyViewer

#undef KMA_VALUE
}
~~~

## Callback manager

An easy way to manage action callback is to use KeyMappingCallbackManager. This manager is a class member, initialize with Context.
One can register callback (std::function<void(QEvent*)>) that are triggered if the input event corresponds to the event binding.

The callback is responsible to test if the event corresponds to an event it can handle.

\note KeyPress and KeyRelease event both trigger the callback associated with a key event, in the callback you can use event->type() to check whether it's a key press or key release (or mouse move) event

Example usage

~~~{.cpp}
// during setup, once keymapping action are initialized
m_keyMappingCallbackManager.addEventCallback( TRACKBALLCAMERA_ZOOM,
                                              [=]( QEvent* event ) { zoomCallback( event ); } );

// the callback is triggered for any binding corresponding to the action, what ever the event type.
// one can check event type to decide how to handle the action, here zoom is handled for both mouse move or wheel
// Note that when zoomCallback is triggered, it corresponds to the config file binding, including key or modifiers, so you don't have to check again.
void TrackballCameraManipulator::zoomCallback( QEvent* event ) {
    if ( event->type() == QEvent::MouseMove ) {
        auto mouseEvent = reinterpret_cast<QMouseEvent*>( event );
        auto [dx, dy]   = computeDeltaMouseMove( mouseEvent );
        handleCameraZoom( dx, dy );
    }
    else if ( event->type() == QEvent::Wheel ) {
        auto wheelEvent = reinterpret_cast<QWheelEvent*>( event );
        handleCameraZoom(
            ( wheelEvent->angleDelta().y() * 0.01_ra + wheelEvent->angleDelta().x() * 0.01_ra ) *
            m_wheelSpeedModifier );
    }
}

// handle the event is then

bool TrackballCameraManipulator::handleMousePressEvent( QMouseEvent* event,
                                                        const Qt::MouseButtons& buttons,
                                                        const Qt::KeyboardModifiers& modifiers,
                                                        int key ) {
// here callback manager check if there an action associated to event,key and then trigger the associated callback.
    bool handled = m_keyMappingCallbackManager.triggerEventCallback( event, key );
    return handled;
}

~~~

## Limits

* Multiple keys/buttons are not yet implemented.
