# ----------------------------------------------------
# This file can be generated from a script:
# To do so, run "./generateFilelistForModule.sh Gui"
# from ./scripts directory
# ----------------------------------------------------

set(gui_sources
    AboutDialog/AboutDialog.cpp
    BaseApplication.cpp
    RadiumWindow/SimpleWindow.cpp
    SelectionManager/SelectionManager.cpp
    SkeletonBasedAnimation/SkeletonBasedAnimationUI.cpp
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
    Utils/KeyMappingManager.cpp
    Utils/Keyboard.cpp
    Utils/PickingManager.cpp
    Viewer/CameraManipulator.cpp
    Viewer/FlightCameraManipulator.cpp
    Viewer/Gizmo/Gizmo.cpp
    Viewer/Gizmo/GizmoManager.cpp
    Viewer/Gizmo/RotateGizmo.cpp
    Viewer/Gizmo/ScaleGizmo.cpp
    Viewer/Gizmo/TranslateGizmo.cpp
    Viewer/RotateAroundCameraManipulator.cpp
    Viewer/TrackballCameraManipulator.cpp
    Viewer/Viewer.cpp
    Viewer/WindowQt.cpp
    Widgets/ControlPanel.cpp
    Widgets/PowerSlider.cpp
    Widgets/CheckingSpinBox.cpp
    Widgets/MatrixEditor.cpp
    MaterialParameterEditor/MaterialParameterEditor.cpp
)

set(gui_headers
    AboutDialog/AboutDialog.hpp
    BaseApplication.hpp
    MainWindowInterface.hpp
    RaGui.hpp
    RadiumWindow/SimpleWindow.hpp
    RadiumWindow/SimpleWindowFactory.hpp
    SelectionManager/SelectionManager.hpp
    SkeletonBasedAnimation/SkeletonBasedAnimationUI.hpp
    Timeline/Configurations.hpp
    Timeline/HelpDialog.hpp
    Timeline/Timeline.hpp
    Timeline/TimelineFrameSelector.hpp
    Timeline/TimelineScrollArea.hpp
    Timeline/TimelineSlider.hpp
    Timeline/TimelineTimeScale.hpp
    TimerData/FrameTimerData.hpp
    TransformEditor/TransformEditor.hpp
    TreeModel/EntityTreeModel.hpp
    TreeModel/TreeModel.hpp
    Utils/KeyMappingManager.hpp
    Utils/Keyboard.hpp
    Utils/PickingManager.hpp
    Utils/qt_utils.hpp
    Viewer/CameraManipulator.hpp
    Viewer/FlightCameraManipulator.hpp
    Viewer/Gizmo/Gizmo.hpp
    Viewer/Gizmo/GizmoManager.hpp
    Viewer/Gizmo/RotateGizmo.hpp
    Viewer/Gizmo/ScaleGizmo.hpp
    Viewer/Gizmo/TranslateGizmo.hpp
    Viewer/RotateAroundCameraManipulator.hpp
    Viewer/TrackballCameraManipulator.hpp
    Viewer/Viewer.hpp
    Viewer/WindowQt.hpp
    Widgets/ControlPanel.hpp
    Widgets/PowerSlider.hpp
    Widgets/CheckingSpinBox.hpp
    Widgets/MatrixEditor.hpp
    MaterialParameterEditor/MaterialParameterEditor.hpp
)

set(gui_inlines Viewer/WindowQt.inl)

set(gui_uis AboutDialog/AboutDialog.ui SkeletonBasedAnimation/SkeletonBasedAnimationUI.ui
            Timeline/HelpDialog.ui Timeline/Timeline.ui
)

set(gui_resources QtResources/RadiumQtResources.qrc
                  SkeletonBasedAnimation/SkeletonBasedAnimation.qrc Timeline/timeline.qrc
)
