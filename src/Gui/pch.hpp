#pragma once

#include <Core/Animation/KeyFramedValueController.hpp>
#include <Core/Animation/KeyFramedValueInterpolators.hpp>
#include <Core/Asset/Camera.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Containers/VariableSetEnumManagement.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/CoreMacros.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Index.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/StringUtils.hpp>
#include <Core/Utils/Timer.hpp>
#include <Engine/Data/Material.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/ForwardRenderer.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>
#include <Engine/Rendering/Renderer.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Engine/Scene/CameraManager.hpp>
#include <Engine/Scene/Component.hpp>
#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/ItemEntry.hpp>
#include <Engine/Scene/Light.hpp>
#include <Engine/Scene/SkeletonBasedAnimationSystem.hpp>
#include <Engine/Scene/SystemDisplay.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>
#include <QApplication>
#include <QBoxLayout>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFrame>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>
#include <QWindow>
#include <algorithm>
#include <atomic>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <ostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>

// raise name resolution issue with glbinding, can be added if gl:: namespace is added everywhere
// #include <QOpenGLContext>
// and ensure include this before
// #include <glbinding/gl/gl.h>
