#ifndef MESHPAINTUI_H
#define MESHPAINTUI_H

#include <QColor>
#include <QFrame>

#include <GuiBase/Utils/PickingManager.hpp>

namespace UI {
class MeshPaintUI;
} // namespace UI

namespace MeshPaintPlugin {
class MeshPaintPluginC;
} // namespace MeshPaintPlugin

namespace Ui {
class MeshPaintUI;
} // namespace Ui

/**
 * The MeshPaintUI class is the Widget for the MeshPaintPlugin.
 */
class MeshPaintUI : public QFrame {
    Q_OBJECT

    friend class MeshPaintPlugin::MeshPaintPluginC;

  public:
    explicit MeshPaintUI( QWidget* parent = nullptr );
    ~MeshPaintUI();

  signals:
    /**
     * Emitted when the user toggles the "Paint Color" checkbox.
     */
    void paintColor( bool );

    /**
     * Emitted when the user changes the color through the color button.
     */
    void colorChanged( const QColor& );

  private slots:
    /**
     * Slot for the user activating the paint.
     */
    void on_paintColor_rb_toggled( bool checked );

    /**
     * Slot for the user changing the paint color.
     */
    void on_changeColor_pb_clicked();

  private:
    /// The actual widget UI.
    Ui::MeshPaintUI* ui;
};

#endif // MESHPAINTUI_H
