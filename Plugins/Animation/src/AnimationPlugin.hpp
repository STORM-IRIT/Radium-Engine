#ifndef ANIMATIONPLUGIN_HPP_
#define ANIMATIONPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
#include <QObject>
#include <QtPlugin>
#include <PluginBase/RadiumPluginInterface.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  Animation_EXPORTS
    #define ANIM_PLUGIN_API DLL_EXPORT
#else
    #define ANIM_PLUGIN_API DLL_IMPORT
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

        virtual void registerPlugin(const Ra::PluginContext& context) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

    public slots:
        void toggleXray( bool on );
        void toggleSkeleton( bool on );
        void step();
        void play();
        void pause();
        void reset();
        void setAnimation( uint i );
        void toggleAnimationTimeStep( bool status );
        void setAnimationSpeed( Scalar value );
        void toggleSlowMotion( bool status );

    private:
        class AnimationSystem* m_system;

    };

} // namespace

#endif // ANIMATIONPLUGIN_HPP_
