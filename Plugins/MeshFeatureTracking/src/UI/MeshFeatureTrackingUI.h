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

    void updateTracking( const Ra::Gui::FeatureData &data,
                         const Ra::Core::Vector3 &pos,
                         const Ra::Core::Vector3 &vec );
private:
    Ui::MeshFeatureTrackingUI *ui;
};

#endif // MESHFEATURETRACKINGUI_H
