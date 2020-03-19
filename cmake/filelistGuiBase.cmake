set( guibase_sources
    BaseApplication.cpp
    KeyFrameEditor/KeyFrameEditor.cpp
    KeyFrameEditor/KeyFrameEditorFrame.cpp
    KeyFrameEditor/KeyFrameEditorFrameScale.cpp
    KeyFrameEditor/KeyFrameEditorScrollArea.cpp
    KeyFrameEditor/KeyFrameEditorTimeScale.cpp
    KeyFrameEditor/KeyFrameEditorValueScale.cpp
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

set( guibase_headers
    BaseApplication.hpp
    MainWindowInterface.hpp
    RaGuiBase.hpp
    KeyFrameEditor/KeyFrameEditor.h
    KeyFrameEditor/KeyFrameEditorFrame.h
    KeyFrameEditor/KeyFrameEditorFrameScale.h
    KeyFrameEditor/KeyFrameEditorScrollArea.h
    KeyFrameEditor/KeyFrameEditorTimeScale.h
    KeyFrameEditor/KeyFrameEditorValueScale.h
    SelectionManager/SelectionManager.hpp
    Timeline/HelpDialog.hpp
    Timeline/Configurations.h
    Timeline/TimelineFrameSelector.h
    Timeline/TimelineTimeScale.h
    Timeline/TimelineSlider.h
    Timeline/TimelineScrollArea.h
    Timeline/Timeline.h
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
    KeyFrameEditor/KeyFrameEditor.ui
    Timeline/HelpDialog.ui
    Timeline/Timeline.ui
    )

set( guibase_resources
    Timeline/timeline.qrc
    )

