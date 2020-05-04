#include <DummyParasitePlugin.hpp>

#include <DummyLibrary.hpp>

namespace MyDummyParasitePlugin {

void DummyParasitePlugin::registerPlugin( const Ra::Plugins::Context& context ) {
    DummyLibrary::DummyMessage( "Registering Parasite DummyPlugin" );
}

} // namespace MyDummyParasitePlugin
