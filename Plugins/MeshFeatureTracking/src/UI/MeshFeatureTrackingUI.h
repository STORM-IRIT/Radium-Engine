#ifndef MESHFEATURETRACKINGUI_H
#define MESHFEATURETRACKINGUI_H

#include <QFrame>
#include <GuiBase/Utils/FeaturePickingManager.hpp>

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

class MeshFeatureTrackingUI : public QFrame
{
    Q_OBJECT

    friend class MeshFeatureTrackingPlugin::MeshFeatureTrackingPluginC;

public:
    explicit MeshFeatureTrackingUI(QWidget *parent = 0);
    ~MeshFeatureTrackingUI();

    void updateTracking();
    void setCurrent( const Ra::Gui::FeatureData &data );

private:
    /// Returns the feature position, i.e. the position for vertices and the barycenter for edges and triangles.
    Ra::Core::Vector3 getFeaturePosition() const;
    /// Returns a vector associated to the feature, i.e. the normal for vertices and triangles, and the direction for edges.
    Ra::Core::Vector3 getFeatureVector() const;

private:
    Ui::MeshFeatureTrackingUI *ui;
    Ra::Gui::FeatureData current;
};

#endif // MESHFEATURETRACKINGUI_H
