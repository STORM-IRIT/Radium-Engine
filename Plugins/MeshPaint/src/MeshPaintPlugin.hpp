#ifndef MESHPAINTPLUGIN_HPP_
#define MESHPAINTPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <Core/Utils/Color.hpp>

#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QColor>
#include <QObject>
#include <QtPlugin>

#include <UI/MeshPaintUI.h>

#include <MeshPaintPluginMacros.hpp>

namespace Ra {
namespace Engine {
class RadiumEngine;
class Entity;
} // namespace Engine
} // namespace Ra

/**
 * The MeshPaintPlugin allows to paint colors on a mesh.
 * For now, it doesn't support textured objects.
 */
namespace MeshPaintPlugin {
class MeshPaintComponent;

// Due to an ambigous name while compiling with Clang, we must differentiate the
// plugin class from plugin namespace
class MeshPaintPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    MeshPaintPluginC();
    ~MeshPaintPluginC() override;

    void registerPlugin( const Ra::PluginContext& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

    bool doAddAction( int& nb ) override;
    QAction* getAction( int id ) override;

  public slots:
    /**
     * Paints on the picked vertices.
     */
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );

    /**
     * Toggle on/off painting.
     */
    void activePaintColor( bool on );

    /**
     * Set the paint color.
     */
    void changePaintColor( const QColor& color );

  private:
    /// The Paint System.
    class MeshPaintSystem* m_system;

    /// The Plugin UI.
    MeshPaintUI* m_widget;

    /// The SelectionManager of the Viewer.
    Ra::GuiBase::SelectionManager* m_selectionManager;

    /// The PickingManager of the Viewer.
    Ra::Gui::PickingManager* m_PickingManager;

    /// The Paint color.
    Ra::Core::Utils::Color m_paintColor;

    /// Whether painting is active or not.
    bool m_isPainting;
};

} // namespace MeshPaintPlugin

#endif // MESHPAINTPLUGIN_HPP_
