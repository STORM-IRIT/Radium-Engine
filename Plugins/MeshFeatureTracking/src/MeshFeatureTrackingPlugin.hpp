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

namespace MeshFeatureTrackingPlugin {
class MeshFeatureTrackingComponent;

// Due to an ambigous name while compiling with Clang, must differentiate plugin class from plugin
// namespace
class MeshFeatureTrackingPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    MeshFeatureTrackingPluginC();
    ~MeshFeatureTrackingPluginC();

    void registerPlugin( const Ra::Plugins::Context& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

    bool doAddAction( int& nb ) override;
    QAction* getAction( int id ) override;

  public slots:
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );
    void update();
    void vertexIdChanged( int );
    void triangleIdChanged( int );

  signals:
    void askForUpdate();

  private:
    MeshFeatureTrackingComponent* m_component{nullptr};
    Ra::GuiBase::SelectionManager* m_selectionManager{nullptr};
    Ra::Gui::PickingManager* m_PickingManager{nullptr};
    MeshFeatureTrackingUI* m_widget{nullptr};
};

} // namespace MeshFeatureTrackingPlugin

#endif // MESHFEATURETRACKINGPLUGIN_HPP_
