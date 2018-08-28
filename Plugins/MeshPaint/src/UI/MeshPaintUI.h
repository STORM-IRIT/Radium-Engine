#ifndef MESHPAINTUI_H
#define MESHPAINTUI_H

#include <QColor>
#include <QFrame>

#include <GuiBase/Utils/PickingManager.hpp>

namespace UI {
class MeshPaintUI;
}

namespace MeshPaintPlugin {
class MeshPaintPluginC;
}

namespace Ui {
class MeshPaintUI;
}

/// The MeshPaintUI classs is the Widget for the MeshPaintPlugin.
class MeshPaintUI : public QFrame {
    Q_OBJECT

    friend class MeshPaintPlugin::MeshPaintPluginC;

  public:
    explicit MeshPaintUI( QWidget* parent = 0 );
    ~MeshPaintUI();

  signals:
    /// Emitted by on_paintColor_rb_toggled.
    void paintColor( bool );

    /// Emitted by on_changeColor_pb_clicked.
    void colorChanged( const QColor& );

  private slots:
    /// Slot for the user activating the paint.
    void on_paintColor_rb_toggled( bool checked );

    /// Slot for the user changing the paint color.
    void on_changeColor_pb_clicked();

  private:
    /// The actual widget UI.
    Ui::MeshPaintUI* ui;
};

#endif // MESHPAINTUI_H
