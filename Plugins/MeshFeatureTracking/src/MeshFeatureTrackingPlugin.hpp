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
// Du to an ambigous name while compiling with Clang, must differentiate plugin class from plugin namespace
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

        virtual bool doAddFeatureTrackingWidget() override;
        virtual QWidget* getFeatureTrackingWidget() override;

    public slots:
        void onCurrentChanged( const QModelIndex& current , const QModelIndex& prev);
        void update();

    private:
        Ra::Engine::Entity* m_entity;
        MeshFeatureTrackingUI* m_widget;

        Ra::GuiBase::SelectionManager* m_selectionManager;
        Ra::Gui::FeaturePickingManager* m_featurePickingManager;
    };

} // namespace

#endif // MESHFEATURETRACKINGPLUGIN_HPP_
