
#include <DummyLibrary.hpp>
#include <PrivateHeader.hpp>

#include <Core/Utils/Log.hpp>

namespace DummyLibrary {
void DummyMessage( const std::string& message ) {
    LOG( Ra::Core::Utils::logINFO ) << DummyPrefix << " -- " << message;
}
} // namespace DummyLibrary
