set( guibase_sources
    BaseApplication.cpp
    RadiumWindow/SimpleWindow.cpp
    SelectionManager/SelectionManager.cpp
    Timeline/HelpDialog.cpp
    Timeline/TimelineFrameSelector.cpp
    Timeline/TimelineTimeScale.cpp
    Timeline/TimelineSlider.cpp
    Timeline/TimelineScrollArea.cpp
    Timeline/Timeline.cpp
    TimerData/FrameTimerData.cpp
    TransformEditor/TransformEditor.cpp
    TreeModel/EntityTreeModel.cpp
    TreeModel/TreeModel.cpp
    Utils/Keyboard.cpp
    Utils/KeyMappingManager.cpp
    Utils/PickingManager.cpp
    Viewer/CameraManipulator.cpp
    Viewer/FlightCameraManipulator.cpp
    Viewer/Gizmo/Gizmo.cpp
    Viewer/Gizmo/GizmoManager.cpp
    Viewer/Gizmo/RotateGizmo.cpp
    Viewer/Gizmo/ScaleGizmo.cpp
    Viewer/Gizmo/TranslateGizmo.cpp
    Viewer/TrackballCameraManipulator.cpp
    Viewer/Viewer.cpp
    Viewer/WindowQt.cpp
)

set(guibase_headers
    BaseApplication.hpp
    MainWindowInterface.hpp
    RaGuiBase.hpp
    RadiumWindow/SimpleWindow.hpp
    RadiumWindow/SimpleWindowFactory.hpp
    SelectionManager/SelectionManager.hpp
    Timeline/HelpDialog.hpp
    Timeline/Configurations.hpp
    Timeline/TimelineFrameSelector.hpp
    Timeline/TimelineTimeScale.hpp
    Timeline/TimelineSlider.hpp
    Timeline/TimelineScrollArea.hpp
    Timeline/Timeline.hpp
    TimerData/FrameTimerData.hpp
    TransformEditor/TransformEditor.hpp
    TreeModel/EntityTreeModel.hpp
    TreeModel/TreeModel.hpp
    Utils/Keyboard.hpp
    Utils/KeyMappingManager.hpp
    Utils/PickingManager.hpp
    Utils/qt_utils.hpp
    Viewer/CameraManipulator.hpp
    Viewer/FlightCameraManipulator.hpp
    Viewer/Gizmo/Gizmo.hpp
    Viewer/Gizmo/GizmoManager.hpp
    Viewer/Gizmo/RotateGizmo.hpp
    Viewer/Gizmo/ScaleGizmo.hpp
    Viewer/Gizmo/TranslateGizmo.hpp
    Viewer/TrackballCameraManipulator.hpp
    Viewer/Viewer.hpp
    Viewer/WindowQt.hpp
)

set( guibase_inlines
   
)

set( guibase_uis
    Timeline/HelpDialog.ui
    Timeline/Timeline.ui
    )

set( guibase_resources
    Timeline/timeline.qrc
    )

