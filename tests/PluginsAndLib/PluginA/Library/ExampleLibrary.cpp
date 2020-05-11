
#include <ExampleLibrary.hpp>
#include <PrivateHeader.hpp>

#include <Core/Utils/Log.hpp>

namespace ExampleLibrary {
void ExampleMessage( const std::string& message ) {
    LOG( Ra::Core::Utils::logINFO ) << ExamplePrefix << " -- " << message;
}
} // namespace ExampleLibrary
