#ifndef PBRTLOADERPLUGIN_HPP_
#define PBRTLOADERPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>
#include <QAction>

#include <PluginBase/RadiumPluginInterface.hpp>

#include <PbrtLoaderPluginMacros.hpp>

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace PbrtLoaderPlugin
{
    class PbrtLoaderPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        virtual ~PbrtLoaderPluginC();

        virtual void registerPlugin( const Ra::PluginContext& context ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

        virtual bool doAddAction( int& nb ) override;
        virtual QAction* getAction( int id ) override;
    };
}


#endif // PBRTLOADERPLUGIN_HPP_
