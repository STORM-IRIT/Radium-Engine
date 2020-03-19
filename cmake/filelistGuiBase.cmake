set( guibase_sources
    BaseApplication.cpp
    SelectionManager/SelectionManager.cpp
    Timeline/qdoublespinboxsmart.cpp
    Timeline/qframebuttons.cpp
    Timeline/qframeselector.cpp
    Timeline/qframetimescale.cpp
    Timeline/qlabelslider.cpp
    Timeline/qscrollarearuler.cpp
    Timeline/qspinboxsmart.cpp
    Timeline/qtoolbuttonplaypause.cpp
    Timeline/qwidgetruler.cpp
    Timeline/Session.cpp
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
    SelectionManager/SelectionManager.hpp
    Timeline/Configurations.h
    Timeline/qdoublespinboxsmart.h
    Timeline/qframebuttons.h
    Timeline/qframeselector.h
    Timeline/qframetimescale.h
    Timeline/qlabelslider.h
    Timeline/qscrollarearuler.h
    Timeline/qspinboxsmart.h
    Timeline/qtoolbuttonplaypause.h
    Timeline/qwidgetruler.h
    Timeline/Session.h
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
    Timeline/Timeline.ui
    )

set( guibase_resources
    Timeline/timeline.qrc
    )

