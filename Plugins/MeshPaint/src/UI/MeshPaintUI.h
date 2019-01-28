#ifndef MESHPAINTUI_H
#define MESHPAINTUI_H

#include <QFrame>
#include <QColor>

#include <GuiBase/Utils/PickingManager.hpp>

namespace UI
{
    class MeshPaintUI;
}

namespace MeshPaintPlugin
{
    class MeshPaintPluginC;
}

namespace Ui
{
    class MeshPaintUI;
}

class MeshPaintUI : public QFrame
{
    Q_OBJECT

    friend class MeshPaintPlugin::MeshPaintPluginC;

public:
    explicit MeshPaintUI(QWidget *parent = 0);
    ~MeshPaintUI();

signals:
    void paintColor( bool );
    void colorChanged( const QColor & );
    void bakeToDiffuse();

private slots:
    void on__bakeToDiffuse_clicked();
    void on_paintColor_rb_toggled( bool checked );
    void on_changeColor_pb_clicked();

private:
    Ui::MeshPaintUI *ui;
};

#endif // MESHPAINTUI_H
