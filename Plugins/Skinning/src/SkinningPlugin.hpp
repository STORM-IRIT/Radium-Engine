#ifndef SKINNINGLUGIN_HPP_
#define SKINNINGLUGIN_HPP_

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  Skinning_EXPORTS
    #define SKIN_PLUGIN_API DLL_EXPORT
#else
    #define SKIN_PLUGIN_API DLL_IMPORT
#endif

#include <QObject>
#include <QtPlugin>
#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace SkinningPlugin
{
// Du to an ambigous name while compiling with Clang, must differentiate plugin claas from plugin namespace
    class SkinningPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        virtual ~SkinningPluginC();

        virtual void registerPlugin( const Ra::PluginContext& context ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

    };

} // namespace

#endif // SKINNINGPLUGIN_HPP_
