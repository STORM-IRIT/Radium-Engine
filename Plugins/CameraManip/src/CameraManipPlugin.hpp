#ifndef CAMERAMANIPPLUGIN_HPP_
#define CAMERAMANIPPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <PluginBase/RadiumPluginInterface.hpp>
#include <QAction>
#include <QColor>
#include <QObject>
#include <QtPlugin>

#include <UI/CameraManipUI.h>

#include <CameraManipPluginMacros.hpp>

namespace Ra {
namespace Engine {
class RadiumEngine;
class Entity;
} // namespace Engine
} // namespace Ra

namespace CameraManipPlugin {

// Due to an ambigous name while compiling with Clang, must differentiate plugin class from plugin
// namespace
/// The CameraManipPlugin allows to change the current camera used for display,
/// create a copy of the current camera and save the current camera data to a file.
class CameraManipPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    CameraManipPluginC();
    virtual ~CameraManipPluginC();

    virtual void registerPlugin( const Ra::PluginContext& context ) override;

    virtual bool doAddWidget( QString& name ) override;
    virtual QWidget* getWidget() override;

    virtual bool doAddMenu() override;
    virtual QMenu* getMenu() override;

    virtual bool doAddAction( int& nb ) override;
    virtual QAction* getAction( int id ) override;

  public slots:
    void useSelectedCamera();
    void saveCamera();
    void createCamera();
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );

  private:
    CameraManipUI* m_widget;

    Ra::Engine::RadiumEngine* m_engine;
    Ra::GuiBase::SelectionManager* m_selectionManager;
    Ra::Gui::Viewer* m_viewer;
};

} // namespace CameraManipPlugin

#endif // CAMERAMANIPPLUGIN_HPP_
