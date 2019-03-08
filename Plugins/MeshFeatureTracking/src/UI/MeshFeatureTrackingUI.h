#ifndef MESHFEATURETRACKINGUI_H
#define MESHFEATURETRACKINGUI_H

#include <GuiBase/Utils/PickingManager.hpp>
#include <QFrame>

#include <MeshFeatureTrackingComponent.hpp>

namespace UI {
class MeshFeatureTrackingUi;
} // namespace UI

namespace MeshFeatureTrackingPlugin {
class MeshFeatureTrackingPluginC;
} // namespace MeshFeatureTrackingPlugin

namespace Ui {
class MeshFeatureTrackingUI;
} // namespace Ui

/**
 * The MeshFeatureTrackingUI class is the Widget for the MeshFeatureTrackingPlugin.
 */
class MeshFeatureTrackingUI : public QFrame {
    Q_OBJECT

    friend class MeshFeatureTrackingPlugin::MeshFeatureTrackingPluginC;

  public:
    explicit MeshFeatureTrackingUI( QWidget* parent = nullptr );
    ~MeshFeatureTrackingUI();

    /**
     * Updates the gui from the given FeatureData, and vertex position and normal
     * if the given feature is a vertex.
     */
    void updateTracking( const MeshFeatureTrackingPlugin::FeatureData& data,
                         const Ra::Core::Vector3& pos, const Ra::Core::Vector3& vec );

    /**
     * Sets the maximal vertex index available.
     */
    void setMaxV( int );

    /**
     * Sets the maximal triangle index available.
     */
    void setMaxT( int );

  signals:
    /**
     * Emitted when the user changes the index of the tracked vertex.
     */
    void vertexIdChanged( int );

    /**
     * Emitted when the user changes the index of the tracked triangle.
     */
    void triangleIdChanged( int );

  private slots:
    /**
     * Slot for the user changing the vertex index.
     */
    void on_m_vertexIdx_valueChanged( int arg1 );

    /**
     * Slot for the user changing the triangle index.
     */
    void on_m_triangleIdx_valueChanged( int arg1 );

  private:
    /// The actual widget UI.
    Ui::MeshFeatureTrackingUI* ui;
};

#endif // MESHFEATURETRACKINGUI_H
