#ifndef DUMMYPLUGIN_DUMMYPLUGIN_HPP
#define DUMMYPLUGIN_DUMMYPLUGIN_HPP

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  Dummy_EXPORTS
    #define DUMMY_PLUGIN_API DLL_EXPORT
#else
    #define DUMMY_PLUGIN_API DLL_IMPORT
#endif

#include <QObject>
#include <QtPlugin>

#include <MainApplication/PluginBase/RadiumPluginInterface.hpp>

class QLabel;

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }
}

namespace DummyPlugin
{
    class DummySystem;

    class DummyPlugin : public QObject, Ra::Plugins::RadiumPluginInterface
    {
        Q_OBJECT
        Q_PLUGIN_METADATA( IID "foo.bar.RadiumEngine.PluginInterface" )
        Q_INTERFACES( Ra::Plugins::RadiumPluginInterface )

    public:
        virtual ~DummyPlugin();

        virtual void registerPlugin( Ra::Engine::RadiumEngine* engine ) override;

        virtual bool doAddWidget( QString& name ) override;
        virtual QWidget* getWidget() override;

        virtual bool doAddMenu() override;
        virtual QMenu* getMenu() override;

    private slots:
        void sayHello();

    private:
        DummySystem* m_system;
        QLabel* m_label;
    };

} // namespace

#endif // FANCYMESHPLUGIN_HPP_
