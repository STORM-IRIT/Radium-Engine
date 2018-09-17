#ifndef CAMERAMANIPUI_H
#define CAMERAMANIPUI_H

#include <QColor>
#include <QFrame>

#include <GuiBase/Utils/PickingManager.hpp>

namespace UI {
class CameraManipUI;
}

namespace CameraManipPlugin {
class CameraManipPluginC;
}

namespace Ui {
class CameraManipUI;
}

class CameraManipUI : public QFrame {
    Q_OBJECT

    friend class CameraManipPlugin::CameraManipPluginC;

  public:
    explicit CameraManipUI( QWidget* parent = 0 );
    ~CameraManipUI();

  public:
    Ui::CameraManipUI* ui;
};

#endif // CAMERAMANIPUI_H
