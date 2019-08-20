\page develkeymapping API: KeyMapping
[TOC]


## Usage

You first need to add into every configuration file (in `Configs/` folder) an entry which will bind your action to a key.
This file as an only `keymaps` node, with `keymap` per binding.

~~~{.xml}
<keymaps>
    <keymap context="CONTEXT" key="KEY" modifiers="MODIFIERS" buttons="BUTTONS" wheel="WHEEL" action="ACTION" />
    <!-- as many entry as needed -->
</keymaps>
~~~

Default values are in \[ \]

* `ACTION` *mandatory* represents the KeyMappingAction enum's value you want to trigger.
* `CONTEXT` \[AppContext\] epresents the context, for instance the class that will try to catch this keymapping
* `KEY` \[no def\] represents the key that need to be pressed to trigger the event (ie `Key_Z`, for example), "" or "-1" or absent correspond to no key needed.
* `MODIFIERS` \[NoModifiers\]represents the modifier used along with key or mouse button (needs to be a `Qt::Modifier` enum value) to trigger the action.
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

// callback when a configuration file is loaded, could check if the context and action are present.
static void configureKeyMapping(){
    m_keyMappingContext = KeyMappingManager::getInstance()->getContext( "MyClassContext" );
    m_myAction =  KeyMappingManager::getInstance()->getActionIndex( "MyActionName" );
}

//[...]
static KeyMappingManager::KeyMappingAction m_myAction;
// [...]
};


// typically in .ccp  file
KeyMappingManager::KeyMappingAction m_myAction;

// then typically in main baseApplication ctor or Viewer ctor, but after KeyMappingManager instance is created :
KeyMappingManager::getInstance()->addListener(MyClass:configureKeyMapping);

~~~

It could be done with a specific macro :

~~~{.cpp}
// in header
#define KeyMappingMyClass \
    KMA_VALUE( MY_ACTION )

#define KMA_VALUE( XX ) static KeyMappingManager::KeyMappingAction XX;
    KeyMappingMyClass
#undef KMA_VALUE

// in source
#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction MyClass::XX;
KeyMappingMyClass
#undef KMA_VALUE

void MyClass::configureKeyMapping() {
    m_keyMappingContext = Gui::KeyMappingManager::getInstance()->getContext( "MyClassContext" );
    if ( m_keyMappingContext.isInvalid() )
        LOG( logINFO ) << "MyClassContext not defined (maybe the configuration file does not contain it";
#define KMA_VALUE( XX ) \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingMyClass
#undef KMA_VALUE
}

~~~

## Implementation note

The viewer is the main entry point to dispatch key and mouse event.
The idea is that at a key press or mouse press event, the viewer is capable of determining which class will receive the events.

For instance see `Viewer.cpp` `Viewer::mousePressEvent`

~~~{.cpp}
//[...]
    auto buttons   = event->buttons();
    auto modifiers = event->modifiers();
    auto key       = activeKey();

    // nothing under mouse ? juste move the camera ...
    if ( result.m_roIdx.isInvalid() )
    {
        auto accepted = m_camera->handleMousePressEvent( event, buttons, modifiers, key );
        if ( accepted ) { m_activeContext = m_camera->getContext(); }
        else
  //[...]
 ~~~

## Limits

* Multiple keys/buttons are not yet implemented.
