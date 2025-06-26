#include <Core/Resources/Resources.hpp>
#include <Core/Utils/StdOptional.hpp>
#include <cpplocate/cpplocate.h>
#include <filesystem>
#include <stack>
#include <string>
#include <utility>

namespace Ra {
namespace Core {
namespace Resources {

using namespace Ra::Core::Utils;
namespace fs = ::std::filesystem;

// see https://www.doxygen.nl/manual/commands.html#cmdcond
/// \cond IMPLEMENTATION_DETAIL
/// wrapper to cpplocate::locatePath to find a path into the filesystem
fs::path searchPath( const std::string& pattern, const std::string& system, void* libSymbol ) {
    std::string p = cpplocate::locatePath( pattern, system, libSymbol );
    return fs::path( p ).lexically_normal();
}

/// Format the path : add a trailing "/" if path point to an existing directory.
fs::path clean( const fs::path& path ) {
    auto status = fs::status( path );
    if ( status.type() == fs::file_type::not_found ) return "";
    if ( status.type() == fs::file_type::directory ) return fs::canonical( path ) / "";
    return fs::canonical( path );
}
/// \endcond

optional<std::string> getRadiumResourcesPath() {
    auto p =
        searchPath( "Resources/Radium/", "", reinterpret_cast<void*>( getRadiumResourcesPath ) );
    p = clean( p / "Resources/Radium" );

    if ( p.empty() ) return {};
    return p.string();
}

optional<std::string> getRadiumPluginsPath() {
    auto p = searchPath( "Plugins/lib", "", reinterpret_cast<void*>( getRadiumPluginsPath ) );
    p      = clean( p / "Plugins" / "lib" );

    if ( p.empty() ) return {};
    return p.string();
}

/// this one is always found, use optional for consistency ?
optional<std::string> getBasePath() {
    return clean( cpplocate::getModulePath() ).string();
}

optional<std::string> getResourcesPath( void* symbol, const std::string& pattern ) {
    auto p = searchPath( pattern, "", symbol );
    p      = clean( p / pattern );
    if ( p.empty() ) return {};
    return p.string();
}

/// \cond IMPLEMENTATION_DETAIL
namespace DataPath {
static std::stack<std::string> s_dataPaths;
}
/// \endcond

std::string getDataPath() {
    if ( DataPath::s_dataPaths.empty() ) { return fs::current_path().string(); }
    return DataPath::s_dataPaths.top();
}

std::string popDataPath() {
    if ( DataPath::s_dataPaths.empty() ) { return fs::current_path().string(); }
    auto p = DataPath::s_dataPaths.top();
    DataPath::s_dataPaths.pop();
    return p;
}

void pushDataPath( std::string datapath ) {
    DataPath::s_dataPaths.emplace( std::move( datapath ) );
    fs::create_directories( DataPath::s_dataPaths.top() );
}

} // namespace Resources
} // namespace Core
} // namespace Ra
