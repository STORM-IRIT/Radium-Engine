#ifndef MESHFEATURETRACKINGUI_H
#define MESHFEATURETRACKINGUI_H

#include <QFrame>
#include <GuiBase/Utils/PickingManager.hpp>

namespace UI
{
    class MeshFeatureTrackingUi;
}

namespace MeshFeatureTrackingPlugin
{
    class MeshFeatureTrackingPluginC;
}

namespace Ui {
class MeshFeatureTrackingUI;
}

struct FeatureData {
    Ra::Engine::Renderer::PickingMode m_mode; // feature type: only vertex, edge, triangles are supported.
    std::array< int, 4 > m_data; // vertices indices in 0-2 according to m_mode, plus triangle idx in 3 for triangles
};

class MeshFeatureTrackingUI : public QFrame
{
    Q_OBJECT

    friend class MeshFeatureTrackingPlugin::MeshFeatureTrackingPluginC;

public:
    explicit MeshFeatureTrackingUI(QWidget *parent = 0);
    ~MeshFeatureTrackingUI();

    void updateTracking( const FeatureData& data,
                         const Ra::Core::Vector3 &pos,
                         const Ra::Core::Vector3 &vec );
    void setMaxV( int );
    void setMaxT( int );

signals:
    void vertexIdChanged( int );
    void triangleIdChanged( int );

private slots:
    void on_m_vertexIdx_valueChanged( int arg1 );
    void on_m_triangleIdx_valueChanged( int arg1 );

private:
    Ui::MeshFeatureTrackingUI *ui;
};

#endif // MESHFEATURETRACKINGUI_H
