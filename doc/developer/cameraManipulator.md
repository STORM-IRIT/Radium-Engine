\page develCameraManipulator API: Camera manipulation in Radium
[TOC]

The Radium Engine provides some classes that allow to extend the way one might interact with a camera in an
application.

The class `Ra::Gui::CameraManipulator` defines the general interface for camera manipulator. All manipulators have
common properties that allow to switch from one manipulator to one other without disturbing the visual and interactive
behavior of the application.

## Implementing a CameraManipulator

In order to extend the set of `Ra::Gui::CameraManipulator` available in Radium or to develop a dedicated
`Ra::Gui::CameraManipulator` for a `Ra::Gui::Viewer`-based application, programmers are intended to do the following,
demonstrated by the class `FlightCameraManipulator`.

1. Define the class that must inherits from `Ra::Gui::CameraManipulator` and, in order to receive interaction events,
from `Ra::Gui::KeyMappingManageable`. Note that a CameraManipulator is a `Q_OBJECT`

~~~{.cpp}
class RA_GUIBASE_API FlightCameraManipulator : public Ra::Gui::CameraManipulator,
                                               public Ra::Gui::KeyMappingManageable<FlightCameraManipulator>
{
    Q_OBJECT
...
};
~~~

2. Implement the constructors (default, copy). Note that it is also very important to implement a constructor
that will take any `Ra::Gui::CameraManipulator` and will copy the base class before initializing the current
manipulator.

~~~{.cpp}
FlightCameraManipulator::FlightCameraManipulator( const Ra::Gui::CameraManipulator& other ) :
    Ra::Gui::CameraManipulator( other ),
    m_rotateAround( true ),
    m_cameraRotateMode( false ),
    m_cameraPanMode( false ),
    m_cameraZoomMode( false ) {
    m_flightSpeed = ( m_target - m_camera->getPosition() ).norm() / 10_ra;
    initializeFixedUpVector();
}

~~~

3. Implement the `Ra::Gui::KeyMappingManageable` part of the class. This implies defining the method
`void FlightCameraManipulator::configureKeyMapping_impl()` according to the semantic imposed
by `Ra::Gui::KeyMappingManageable`. It is recommended that, when implementing this keymapping initialisation callback,
a default configuration is defined and saved to the xml keymapping configuration file if this later does not already
contains a configuration. This will allow the users to edit and customize the proposed keymapping configuration.

~~~{.cpp}
#define KMA_VALUE( XX ) Gui::KeyMappingManager::KeyMappingAction Gui::FlightCameraManipulator::XX;
KeyMappingFlightManipulator
#undef KMA_VALUE

    void FlightCameraManipulator::configureKeyMapping_impl() {
    m_keyMappingContext =
        Ra::Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" );
    if ( m_keyMappingContext.isInvalid() )
    {
        // The context is undefined, add a default configuration to the keymapping stystem
        Ra::Gui::KeyMappingManager::getInstance()->addAction(
            "FlightManipulatorContext", "", "", "LeftButton", "", "FLIGHTMODECAMERA_ROTATE" );
        Ra::Gui::KeyMappingManager::getInstance()->addAction( "FlightManipulatorContext",
                                                          "",
                                                          "ShiftModifier",
                                                          "LeftButton",
                                                          "",
                                                          "FLIGHTMODECAMERA_PAN" );
        Ra::Gui::KeyMappingManager::getInstance()->addAction( "FlightManipulatorContext",
                                                          "",
                                                          "ControlModifier",
                                                          "LeftButton",
                                                          "",
                                                          "FLIGHTMODECAMERA_ZOOM" );
        Ra::Gui::KeyMappingManager::getInstance()->addAction(
            "FlightManipulatorContext", "Key_A", "", "", "", "FLIGHTMODECAMERA_ROTATE_AROUND" );
        m_keyMappingContext =
            Ra::Gui::KeyMappingManager::getInstance()->getContext( "FlightManipulatorContext" );
    }

#define KMA_VALUE( XX ) \
    XX = Ra::Gui::KeyMappingManager::getInstance()->getActionIndex( m_keyMappingContext, #XX );
    KeyMappingFlightManipulator
#undef KMA_VALUE
}
~~~

4. Implement the inherited abstract method according to the wanted behavior of the `Ra::Gui::CameraManipulator`

## Using a CameraManipulator
Using a `Ra::Gui::CameraManipulator` in a `Ra::Gui::Viewer`-based application is quite straightforward.

If one wants to set a first camera manipulator to a viewer
~~~{.cpp}
myViewer->setCameraManipulator(
        new FlightCameraManipulator( width, height );
~~~

If one wants to change the manipulator while keeping the actual visual state
~~~{.cpp}
myViewer->setCameraManipulator(
        new FlightCameraManipulator( *( m_viewer->getCameraManipulator() ) ) );
~~~
