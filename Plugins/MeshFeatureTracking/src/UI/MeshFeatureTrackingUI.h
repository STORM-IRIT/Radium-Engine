#ifndef MESHFEATURETRACKINGUI_H
#define MESHFEATURETRACKINGUI_H

#include <GuiBase/Utils/PickingManager.hpp>
#include <QFrame>

namespace UI {
class MeshFeatureTrackingUi;
}

namespace MeshFeatureTrackingPlugin {
class MeshFeatureTrackingPluginC;
}

namespace Ui {
class MeshFeatureTrackingUI;
}

/// The FeatureData struct stores the feature informations to display.
struct FeatureData {
    /// The feature type: only vertex, edge, triangles are supported.
    Ra::Engine::Renderer::PickingMode m_mode;
    /// The vertices indices in 0-2 according to m_mode, plus triangle idx in 3 for triangles
    std::array<int, 4> m_data;
};

/// The MeshFeatureTrackingUI class is the Widget for the MeshFeatureTrackingPlugin.
class MeshFeatureTrackingUI : public QFrame {
    Q_OBJECT

    friend class MeshFeatureTrackingPlugin::MeshFeatureTrackingPluginC;

  public:
    explicit MeshFeatureTrackingUI( QWidget* parent = 0 );
    ~MeshFeatureTrackingUI();

    /// updates the UI from the given FeatureData, and vertex position and normal
    /// if the given feature is a vertex.
    void updateTracking( const FeatureData& data, const Ra::Core::Vector3& pos,
                         const Ra::Core::Vector3& vec );

    /// Sets the maximal vertex index available.
    void setMaxV( int );

    /// Sets the maximal triangle index available.
    void setMaxT( int );

  signals:
    /// Emitted by on_m_vertexIdx_valueChanged.
    void vertexIdChanged( int );

    /// Emitted by on_m_triangleIdx_valueChanged.
    void triangleIdChanged( int );

  private slots:
    /// Slot for the user changing the vertex index.
    void on_m_vertexIdx_valueChanged( int arg1 );

    /// Slot for the user changing the triangle index.
    void on_m_triangleIdx_valueChanged( int arg1 );

  private:
    /// The actual widget UI.
    Ui::MeshFeatureTrackingUI* ui;
};

#endif // MESHFEATURETRACKINGUI_H
