#include <ExampleLibrary.hpp>
#include <PluginA.hpp>
#include <string>

namespace PluginA_NS {

void PluginA::registerPlugin( const Ra::Plugins::Context& ) {
    ExampleLibrary::ExampleMessage( "Registering pluginA" );
}

} // namespace PluginA_NS
