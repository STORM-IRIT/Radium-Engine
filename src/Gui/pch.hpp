#pragma once

#include <Core/Asset/Camera.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/CoreMacros.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Index.hpp>
#include <Core/Utils/Log.hpp>
#include <Core/Utils/StringUtils.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>
#include <Engine/Rendering/Renderer.hpp>
#include <Engine/Scene/CameraComponent.hpp>
#include <Engine/Scene/Component.hpp>
#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/ItemEntry.hpp>
#include <Engine/Scene/Light.hpp>
#include <Engine/Scene/SystemDisplay.hpp>
#include <QApplication>
#include <QDialog>
#include <QFrame>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QTimer>
#include <atomic>
#include <iostream>
#include <memory>
#include <vector>
// raise name resolution issue with glbinding, can be added if gl:: namespace is added everywhere
// #include <QOpenGLContext>
// and ensure include this before
// #include <glbinding/gl/gl.h>
