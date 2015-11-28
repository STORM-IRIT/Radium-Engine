#ifndef FANCYMESHPLUGIN_HPP_
#define FANCYMESHPLUGIN_HPP_ 

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  FancyMesh_EXPORTS    
    #define FM_PLUGIN_API DLL_EXPORT 
#elif defined FancyMesh_IMPORTS
    #define FM_PLUGIN_API DLL_IMPORT 
#else
    #define FM_PLUGIN_API 
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

namespace FancyMeshPlugin
{
    class FancyMeshPlugin : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "foo.bar.RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        virtual ~FancyMeshPlugin();

        virtual void registerPlugin( Ra::Engine::RadiumEngine* engine ) override;
        virtual void setupInterface() override;
    };

} // namespace

#endif // FANCYMESHPLUGIN_HPP_  
