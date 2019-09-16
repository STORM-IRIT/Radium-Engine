# Camera manipulation in Radium

The Radium Engine provides some classes that allow to extend the way one might interact with a camera in an application.

The class `Ra::Gui::CameraManipulator` define the general interface for camera manipulator. All manipulators have
common properties that allow to switch from one manipulator to one other without disturbing the visual and interactive
behavior of the application.

## Implementing a CameraManipulator

In order to extend the set of `CameraManipulator` available in Radium or to develop a dedicated `CameraManipulator` 
for a `Viewer`-based application, programmers are intended to do the following, demonstrated by the 
class `FlightCameraManipulator`.

1. Define the class that must inherits from `CameraManipulator` and, in order to receive interaction events, from 
 `KeyMappingManageable`. Note that a CameraManipulator is a `Q_OBJECT` 

```c++
class RA_GUIBASE_API FlightCameraManipulator : public CameraManipulator,
                                               public KeyMappingManageable<FlightCameraManipulator>
{
    Q_OBJECT
...
} 
```

2. Implement the constructors (default, copy). Note that it is also very important to implement a constructor 
that will take any `CameraManipulator` and will copy the base class befor initializing the current manipulator.

```c++
Gui::FlightCameraManipulator::FlightCameraManipulator( const CameraManipulator& other ) :
    CameraManipulator( other ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    m_flightSpeed = ( m_target - m_camera->getPosition() ).norm() / 10_ra;
    initializeFixedUpVector();
}

```

3. Implement the `KeyMappingManageable` part of the class. This implies defining the method 
`void Gui::FlightCameraManipulator::configureKeyMapping_impl()` according to the sementic imposed 
by `KeyMappingManageable`. It is recommended that, when implementing this keymapping initialisation callback, a default 
configuration is defined and saved to the xml keymapping configuration file if this later does not already contains a 
configuration. This will allow the users to edit and customize the proposed keymapping configuration.

```c++
#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction Gui::FlightCameraManipulator::XX;
KeyMappingFlightManipulator
#undef KMA_VALUE

    void
    Gui::FlightCameraManipulator::configureKeyMapping_impl() {

    m_keyMappingContext =
        Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" );
    if ( m_keyMappingContext.isInvalid() )
    {
        // The context is undefined, add a default configuration to the keymapping stystem 
        Gui::KeyMappingManager::getInstance()->addAction(
            "FlightManipulatorContext", "", "", "LeftButton", "", "FLIGHTMODECAMERA_ROTATE" );
        Gui::KeyMappingManager::getInstance()->addAction( "FlightManipulatorContext",
                                                          "",
                                                          "ShiftModifier",
                                                          "LeftButton",
                                                          "",
                                                          "FLIGHTMODECAMERA_PAN" );
        Gui::KeyMappingManager::getInstance()->addAction( "FlightManipulatorContext",
                                                          "",
                                                          "ControlModifier",
                                                          "LeftButton",
                                                          "",
                                                          "FLIGHTMODECAMERA_ZOOM" );
        Gui::KeyMappingManager::getInstance()->addAction(
            "FlightManipulatorContext", "Key_A", "", "", "", "FLIGHTMODECAMERA_ROTATE_AROUND" );
        m_keyMappingContext =
            Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" );
    }

#define KMA_VALUE( XX ) \
    XX = Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingFlightManipulator
#undef KMA_VALUE
}
```

4. Implement the inherited abstract method according to the wanted behavior of the `CameraManipulator`

## Using a CameraManipulator
Using a `CameraManipulator` in a `Viewer`-based application is quite straightforward. 

If one want to set a first camera manipulator to a viewer
```c++
myViewer->setCameraManipulator(
        new Gui::FlightCameraManipulator( width, height );
```

If one want to change the manipulator while keeping the actual visual state 
```c++
myViewer->setCameraManipulator(
        new Gui::FlightCameraManipulator( *( m_viewer->getCameraManipulator() ) ) );
```
