#include <Core/Resources/Resources.hpp>

#include <Core/Utils/Log.hpp>
#include <algorithm>
#include <cpplocate/cpplocate.h>

#ifndef CXX_FILESYSTEM_HAVE_FS
#    error std::filesystem is required to compile this file
#endif
#if CXX_FILESYSTEM_IS_EXPERIMENTAL
#    include <experimental/filesystem>

// We need the alias from std::experimental::filesystem to std::filesystem
namespace std {
namespace filesystem = experimental::filesystem;
}

#else
#    include <filesystem>
#endif

namespace Ra {
namespace Core {
namespace Resources {

std::filesystem::path
searchPath( const std::string& pattern, const std::string& system, void* libSymbol ) {
    std::string basePath = cpplocate::locatePath( pattern, system, libSymbol );
    return std::filesystem::path( basePath ).lexically_normal();
}

// add a trailing "/" if path point to an existing directory.
std::filesystem::path clean( const std::filesystem::path& path ) {
    auto status = std::filesystem::status( path );
    if ( status.type() == std::filesystem::file_type::not_found ) return "";
    if ( status.type() == std::filesystem::file_type::directory ) return path / "";
    return path;
}

std::string getRadiumResourcesPath() {
    auto basePath =
        searchPath( "Resources/Shaders", "", reinterpret_cast<void*>( getRadiumResourcesPath ) );

    basePath = clean( basePath / "Resources" );

    return ( basePath ).string();
}

std::string getRadiumPluginsPath() {
    auto basePath =
        searchPath( "Plugins/lib", "", reinterpret_cast<void*>( getRadiumPluginsPath ) );
    basePath = clean( basePath / "Plugins" / "lib" );
    return basePath.string();
}

std::string getBasePath() {
    return clean( cpplocate::getModulePath() );
}

std::string getResourcesPath( void* symbol, const std::string& pattern ) {
    auto basePath = searchPath( pattern, "", symbol );
    basePath      = clean( basePath / pattern );
    return ( basePath ).string();
}

} // namespace Resources
} // namespace Core
} // namespace Ra
