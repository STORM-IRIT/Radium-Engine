set( guibase_sources
    BaseApplication.cpp
    RadiumWindow/SimpleWindow.cpp
    SelectionManager/SelectionManager.cpp
    Timeline/HelpDialog.cpp
    Timeline/Timeline.cpp
    Timeline/TimelineFrameSelector.cpp
    Timeline/TimelineScrollArea.cpp
    Timeline/TimelineSlider.cpp
    Timeline/TimelineTimeScale.cpp
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

set( guibase_headers
    BaseApplication.hpp
    MainWindowInterface.hpp
    RadiumWindow/SimpleWindowFactory.hpp
    RadiumWindow/SimpleWindow.hpp
    RaGuiBase.hpp
    SelectionManager/SelectionManager.hpp
    Timeline/Configurations.hpp
    Timeline/HelpDialog.hpp
    Timeline/TimelineFrameSelector.hpp
    Timeline/Timeline.hpp
    Timeline/TimelineScrollArea.hpp
    Timeline/TimelineSlider.hpp
    Timeline/TimelineTimeScale.hpp
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

