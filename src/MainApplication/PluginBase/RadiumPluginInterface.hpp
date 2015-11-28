#ifndef RADIUM_RADIUMPLUGININTERFACE_HPP
#define RADIUM_RADIUMPLUGININTERFACE_HPP

class QWidget;

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
    }

    namespace Plugins
    {
        class RadiumPluginInterface
        {
        public:
            virtual ~RadiumPluginInterface() {}

            virtual void registerPlugin( Engine::RadiumEngine* engine ) = 0;
            virtual void setupInterface( QWidget*  ) = 0;
        };
    }
}

#define RadiumPluginInterface_IID "foo.bar.RadiumEngine.PluginInterface"
Q_DECLARE_INTERFACE( Ra::Plugins::RadiumPluginInterface, RadiumPluginInterface_IID )


#endif // RADIUM_RADIUMPLUGININTERFACE_HPP
