#ifndef RADIUM_RADIUMPLUGININTERFACE_HPP
#define RADIUM_RADIUMPLUGININTERFACE_HPP

namespace Ra
{
    namespace Plugins
    {
        class RadiumPluginInterface
        {
        public:
            virtual ~RadiumPluginInterface() {}

            virtual void registerPlugin( /* engine */ ) = 0;
            virtual void setupInterface( /* widget tab, actions */ ) = 0;
        };
    }
}

#define RadiumPluginInterface_IID "foo.bar.RadiumEngine.PluginInterface"
Q_DECLARE_INTERFACE( Ra::Plugins::RadiumPluginInterface, RadiumPluginInterface_IID )


#endif // RADIUM_RADIUMPLUGININTERFACE_HPP
