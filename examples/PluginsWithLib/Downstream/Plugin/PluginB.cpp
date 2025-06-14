#include <ExampleLibrary.hpp>
#include <PluginB.hpp>
#include <string>

namespace PluginB_NS {

void PluginB::registerPlugin( const Ra::Plugins::Context& ) {
    ExampleLibrary::ExampleMessage( "Registering PluginB *** " );
}

} // namespace PluginB_NS
