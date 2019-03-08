#ifndef MESHFEATURETRACKINGPLUGIN_HPP_
#define MESHFEATURETRACKINGPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QObject>
#include <QtPlugin>

#include <UI/MeshFeatureTrackingUI.h>

#include <MeshFeatureTrackingPluginMacros.hpp>

namespace Ra {
namespace Engine {
class RadiumEngine;
class Entity;
} // namespace Engine
} // namespace Ra

/**
 * The MeshFeatureTrackingPlugin displays a sphere around the selected mesh feature.
 * Such a feature is either a vertex, an edge or a triangle. This plugin also displays
 * information on the feature, such as its index, position and normal vector.
 */
namespace MeshFeatureTrackingPlugin {
class MeshFeatureTrackingComponent;

// Due to an ambigous name while compiling with Clang, we must differentiate the
// plugin class from plugin namespace
class MeshFeatureTrackingPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    MeshFeatureTrackingPluginC();
    ~MeshFeatureTrackingPluginC() override;

    void registerPlugin( const Ra::PluginContext& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

    bool doAddAction( int& nb ) override;
    QAction* getAction( int id ) override;

  public slots:
    /**
     * Updates the FeatureData from the lattest selection.
     */
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );

    /**
     * Updates the ui and the sphere.
     */
    void update();

    /**
     * Slot for the user changing the vertex index.
     */
    void vertexIdChanged( int );

    /**
     * Slot for the user changing the triangle index.
     */
    void triangleIdChanged( int );

  private:
    /// The MeshFeatureTrackingComponent displaying the sphere.
    MeshFeatureTrackingComponent* m_component;

    /// The Plugin UI.
    MeshFeatureTrackingUI* m_widget;

    /// The SelectionManager of the Viewer.
    Ra::GuiBase::SelectionManager* m_selectionManager;

    /// The PickingManager of the Viewer.
    Ra::Gui::PickingManager* m_PickingManager;
};

} // namespace MeshFeatureTrackingPlugin

#endif // MESHFEATURETRACKINGPLUGIN_HPP_
