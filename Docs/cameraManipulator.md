# Camera manipulation in Radium

The Radium Engine provides some classes that allow to extend the way one might interact with a camera in an application.

The class `Ra::Gui::CameraManipulator` define the general interface for camera manipulator. All manipulators have
common properties that allow to switch from one manipulator to one other without disturbing the visual and interactive
behavior of the application.

In order to extend the set of `CameraManipulator` available in Radium or to develop a dedicated `CameraManipulator` 
for a `Viewer`-based application, programmers are intended to do the following, demonstrated by the 
class `FlightCameraManipulator`.
