#include <ExampleLibrary.hpp>
#include <PluginA.hpp>
#include <memory>

namespace Ra {
namespace Plugins {
class Context;
} // namespace Plugins
} // namespace Ra

namespace PluginA_NS {

void PluginA::registerPlugin( const Ra::Plugins::Context& ) {
    ExampleLibrary::ExampleMessage( "Registering pluginA" );
}

} // namespace PluginA_NS
