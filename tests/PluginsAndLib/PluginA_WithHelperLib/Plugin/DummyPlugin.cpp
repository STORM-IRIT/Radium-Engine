#include <DummyPlugin.hpp>

#include <DummyLibrary.hpp>

namespace MyDummyPlugin {

void DummyPlugin::registerPlugin( const Ra::Plugins::Context& context ) {
    DummyLibrary::DummyMessage( "Registering DummyPlugin" );
}

} // namespace MyDummyPlugin
