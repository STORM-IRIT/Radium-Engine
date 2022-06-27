#include <PluginA.hpp>

#include <ExampleLibrary.hpp>

namespace PluginA_NS {

void PluginA::registerPlugin( const Ra::Plugins::Context& ) {
    ExampleLibrary::ExampleMessage( "Registering pluginA" );
}

} // namespace PluginA_NS
