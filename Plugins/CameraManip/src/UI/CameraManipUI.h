#ifndef CAMERAMANIPUI_H
#define CAMERAMANIPUI_H

#include <QFrame>

#include <GuiBase/Utils/PickingManager.hpp>

namespace UI {
class CameraManipUI;
} // namespace UI

namespace CameraManipPlugin {
class CameraManipPluginC;
} // namespace CameraManipPlugin

namespace Ui {
class CameraManipUI;
} // namespace Ui

/**
 * The CameraManipUI class is the Widget for the CameraManipPlugin.
 */
class CameraManipUI : public QFrame {
    Q_OBJECT

    friend class CameraManipPlugin::CameraManipPluginC;

  public:
    explicit CameraManipUI( QWidget* parent = nullptr );
    ~CameraManipUI();

  public:
    Ui::CameraManipUI* ui;
};

#endif // CAMERAMANIPUI_H
