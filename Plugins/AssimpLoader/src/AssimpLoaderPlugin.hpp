#ifndef ASSIMPLOADERPLUGIN_HPP_
#define ASSIMPLOADERPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>
#include <QAction>

#include <PluginBase/RadiumPluginInterface.hpp>

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace AssimpLoaderPlugin
{
    class AssimpLoaderPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        virtual ~AssimpLoaderPluginC();

        virtual void registerPlugin( const Ra::PluginContext& context ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

        virtual bool doAddAction( int& nb ) override;
        virtual QAction* getAction( int id ) override;
    };
}


#endif //ASSIMPLOADERPLUGIN_HPP_
