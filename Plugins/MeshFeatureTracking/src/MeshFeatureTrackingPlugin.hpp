#ifndef MESHFEATURETRACKINGPLUGIN_HPP_
#define MESHFEATURETRACKINGPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <QObject>
#include <QtPlugin>
#include <QAction>
#include <PluginBase/RadiumPluginInterface.hpp>

#include <UI/MeshFeatureTrackingUI.h>

#include <MeshFeatureTrackingPluginMacros.hpp>

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
        class Entity;
    }
}

namespace MeshFeatureTrackingPlugin
{
    class MeshFeatureTrackingComponent;

    // Due to an ambigous name while compiling with Clang, must differentiate plugin class from plugin namespace
    class MeshFeatureTrackingPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        MeshFeatureTrackingPluginC();
        virtual ~MeshFeatureTrackingPluginC();

        virtual void registerPlugin(const Ra::PluginContext& context) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

        virtual bool doAddAction( int& nb ) override;
        virtual QAction* getAction( int id ) override;

    public slots:
        void onCurrentChanged( const QModelIndex& current , const QModelIndex& prev);
        void update();
        void vertexIdChanged( int );
        void triangleIdChanged( int );

    private:
        MeshFeatureTrackingComponent* m_component;
        MeshFeatureTrackingUI* m_widget;

        Ra::GuiBase::SelectionManager* m_selectionManager;
        Ra::Gui::PickingManager* m_PickingManager;
    };

} // namespace

#endif // MESHFEATURETRACKINGPLUGIN_HPP_
