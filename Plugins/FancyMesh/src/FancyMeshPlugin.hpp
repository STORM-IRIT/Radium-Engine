#ifndef FANCYMESHPLUGIN_HPP_
#define FANCYMESHPLUGIN_HPP_

#include <QObject>
#include <QtPlugin>
#include <QAction>

#include <PluginBase/RadiumPluginInterface.hpp>

#include <FancyMeshPluginMacros.hpp>

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace FancyMeshPlugin
{
    // Du to an ambigous name while compiling with Clang, must differentiate plugin claas from plugin namespace
    class FancyMeshPluginC : public QObject, public Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        virtual ~FancyMeshPluginC();

        void registerPlugin( const Ra::PluginContext& context ) override;
        
        bool doAddWidget( QString& name ) override;
        QWidget* getWidget() override;
        
        bool doAddMenu() override;
        QMenu* getMenu() override;
        
        bool doAddAction( int& nb ) override;
        QAction* getAction( int id ) override;
    };

} // namespace

#endif // FANCYMESHPLUGIN_HPP_
