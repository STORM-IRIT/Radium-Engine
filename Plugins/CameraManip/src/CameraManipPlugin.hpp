#ifndef CAMERAMANIPPLUGIN_HPP_
#define CAMERAMANIPPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>
#include <QObject>

#include <UI/CameraManipUI.h>

#include <CameraManipPluginMacros.hpp>

namespace Ra {
namespace Engine {
class RadiumEngine;
class Entity;
} // namespace Engine
} // namespace Ra

/**
 * The CameraManipPlugin allows to change the current camera used for display,
 * create a copy of the current camera and save the current camera data to a file.
 */
namespace CameraManipPlugin {

// Due to an ambigous name while compiling with Clang, we must differentiate the
// plugin class from plugin namespace
class CameraManipPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    CameraManipPluginC();
    ~CameraManipPluginC() override;

    void registerPlugin( const Ra::PluginContext& context ) override;

    bool doAddWidget( QString& name ) override;
    QWidget* getWidget() override;

    bool doAddMenu() override;
    QMenu* getMenu() override;

    bool doAddAction( int& nb ) override;
    QAction* getAction( int id ) override;

  public slots:
    /**
     * Use the selected camera, if any, as the viewer camera.
     */
    void useSelectedCamera();

    /**
     * Exports the selected camera data to file.
     */
    void saveCamera();

    /**
     * Copy the viewer camera to create a new one.
     */
    void createCamera();

    /**
     * Deal with object selection.
     */
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );

  private:
    /// The Plugin UI.
    CameraManipUI* m_widget;

    /// The RadiumEngine the cameras belong to.
    Ra::Engine::RadiumEngine* m_engine;

    /// The SelectionManager of the Viewer.
    Ra::GuiBase::SelectionManager* m_selectionManager;

    /// The Viewer.
    Ra::Gui::Viewer* m_viewer;
};

} // namespace CameraManipPlugin

#endif // CAMERAMANIPPLUGIN_HPP_
