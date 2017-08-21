#ifndef RADIUMENGINE_POINTCLOUDPLUGIN_HPP
#define RADIUMENGINE_POINTCLOUDPLUGIN_HPP

#include <QObject>
#include <QtPlugin>
#include <QAction>

#include <PluginBase/RadiumPluginInterface.hpp>

#include <PointCloudPluginMacros.hpp>
#include <UI/PointCloudUI.h>
#include "ui_PointCloudUI.h"


namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace PointCloudPlugin
{
// Du to an ambigous name while compiling with Clang, must differentiate plugin claas from plugin namespace
    class PointCloudPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        PointCloudPluginC();
        virtual ~PointCloudPluginC();

        virtual void registerPlugin( const Ra::PluginContext& context ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

        virtual bool doAddAction( int& nb ) override;
        virtual QAction* getAction( int id ) override;

    public slots:
        void changeNeighSize (int size);
        void changeShowPos (bool showPos);
        void changePlaneFit (bool planeFit);
        void changeDepthThresh (double dThresh);
        void changeRadius (double radius);
        void changeDepthCalc(int index);

    private:
        class PointCloudSystem* m_system;
        PointCloudUI* m_widget;
        Ra::GuiBase::SelectionManager* m_selectionManager;
    };

} // namespace

#endif //RADIUMENGINE_POINTCLOUDPLUGIN_HPP
