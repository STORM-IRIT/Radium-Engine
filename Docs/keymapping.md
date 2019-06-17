# KeyMapping Manual

## Usage

You can find KeyMappingAction enumeration in `KeyMappingManager.hpp` which contains all of the actions you can handle. If you wish to add an action, just add a value to the enum (try to keep it clear).

Then, you will need to add into every configuration file (`Configs/` folder) an entry which will bind your action to a key :

```xml
<keymaps>
    <keymap key="KEY" modifiers="MODIFIERS" buttons="BUTTONS" action="ACTION" />
    <!-- as many entry as needed -->
</keymaps>
```

* `KEY` represents the key that need to be pressed to trigger the event (ie `Key_Z`, for example), "" or "-1" or absent correspond to no key needed.
* `MODIFIERS` represents the modifier used along with key or mouse button (needs to be a `Qt::Modifier` enum value) to trigger the action.
* `BUTTONS` represents the button to trigger the event (i.e. `LeftButton`, for example).
* `ACTION` represents the KeyMappingAction enum's value you want to trigger.

If only a key is defined then it's a `keyPressedEvent`. 
If buttons is defined, then it's a `mouse[move/press/release]Event`, that optionally take modifiers and key into account.

## Implementation note

Try to limit keyMapping to your Viewer class. Then event of the viewer class call the corresponding methods of your other classes.

For instance see `Viewer.cpp` `Viewer::mousePressEvent`

```c++
// [...]
    auto keyMap    = Gui::KeyMappingManager::getInstance();
    auto buttons   = event->buttons();
    auto modifiers = event->modifiers();
    auto key       = lastActiveKey();
    auto action    = keyMap->getAction( buttons, modifiers, key );

    if ( action == Gui::KeyMappingManager::TRACKBALLCAMERA_MANIPULATION )
    { m_camera->handleMousePressEvent( event ); }
// [...]

 ```  

## Limits

* Multiple keys/modifiers/buttons are not yet implemented.
