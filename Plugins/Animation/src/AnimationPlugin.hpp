#ifndef ANIMATIONPLUGIN_HPP_
#define ANIMATIONPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <QObject>
#include <QtPlugin>
#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  Animation_EXPORTS
    #define ANIM_PLUGIN_API DLL_EXPORT
#elif defined Animation_IMPORTS
    #define ANIM_PLUGIN_API DLL_IMPORT
#else
    #define ANIM_PLUGIN_API
#endif


namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace AnimationPlugin
{
// Du to an ambigous name while compiling with Clang, must differentiate plugin class from plugin namespace
    class AnimationPluginC : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        AnimationPluginC();
        virtual ~AnimationPluginC();

        virtual void registerPlugin( Ra::Engine::RadiumEngine* engine ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

    public slots:
        void toggleXray( bool on );
        void step();
    private:
        class AnimationSystem* m_system;

    };

} // namespace

#endif // ANIMATIONPLUGIN_HPP_
