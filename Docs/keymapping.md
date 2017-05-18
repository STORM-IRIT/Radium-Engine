# KeyMapping Manual

## Usage

You can find KeyMappingAction enumeration in `KeyMappingManager.hpp` which contains all of the actions you can handle. If you wish to add an action, just add a value to the enum (try to keep it clear, it currently uses FILENAME_ACTION_NAME notation).

Then, you will need to add into every configuration file (`Configs/` folder) an entry which will bind your action to a key :

    <keymap>
        <information id="type" modifier="MODIFIER" type="TYPE" value="VALUE"></information>
        <information id="action" value="FILENAME_ACTION_NAME"></information>
    </keymap>

* MODIFIER represents the modifier used (needs to be a `Qt::Modifier` enum value) to trigger the action.
* TYPE represents the type of event used to trigger the action (can be `mouse` or `key`).
* VALUE represents the value of the type you want in order to trigger the action (i.e. `LeftButton` if type is mouse or `Key_Z` if type is key, for example).
* FILENAME_ACTION_NAME represents the KeyMappingAction enum's value you want to trigger.

## Constraints

Some KeyMappingAction enumeration's values may be dependant. It is currently the case with :

* `VIEWER_LEFT_BUTTON_PICKING_QUERY` and `GIZMOMANAGER_MANIPULATION`

## Limits

* Modifier value isn't used if you're using the `mouse` type, you can just ignore this field for this type.