#include <PluginB.hpp>

#include <ExampleLibrary.hpp>

namespace PluginB_NS {

void PluginB::registerPlugin( const Ra::Plugins::Context& ) {
    ExampleLibrary::ExampleMessage( "Registering PluginB *** " );
}

} // namespace PluginB_NS
