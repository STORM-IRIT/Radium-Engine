#ifndef MESHPAINTPLUGIN_HPP_
#define MESHPAINTPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
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

namespace MeshPaintPlugin {
class MeshPaintComponent;

// Due to an ambigous name while compiling with Clang, must differentiate plugin class from plugin
// namespace
class MeshPaintPluginC : public QObject, Ra::Plugins::RadiumPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
    Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

  public:
    MeshPaintPluginC();
    virtual ~MeshPaintPluginC();

    virtual void registerPlugin( const Ra::PluginContext& context ) override;

    virtual bool doAddWidget( QString& name ) override;
    virtual QWidget* getWidget() override;

    virtual bool doAddMenu() override;
    virtual QMenu* getMenu() override;

    virtual bool doAddAction( int& nb ) override;
    virtual QAction* getAction( int id ) override;

  public slots:
    void onCurrentChanged( const QModelIndex& current, const QModelIndex& prev );
    void activePaintColor( bool on );
    void changePaintColor( const QColor& color );

  private:
    MeshPaintUI* m_widget;

    Ra::GuiBase::SelectionManager* m_selectionManager;
    Ra::Gui::PickingManager* m_PickingManager;

    class MeshPaintSystem* m_system;

    Ra::Core::Math::Color m_paintColor;
    bool m_isPainting;
};

} // namespace MeshPaintPlugin

#endif // MESHPAINTPLUGIN_HPP_
