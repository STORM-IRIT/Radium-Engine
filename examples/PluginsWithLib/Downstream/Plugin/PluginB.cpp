#include <ExampleLibrary.hpp>
#include <PluginB.hpp>
#include <memory>

namespace Ra {
namespace Plugins {
class Context;
} // namespace Plugins
} // namespace Ra

namespace PluginB_NS {

void PluginB::registerPlugin( const Ra::Plugins::Context& ) {
    ExampleLibrary::ExampleMessage( "Registering PluginB *** " );
}

} // namespace PluginB_NS
