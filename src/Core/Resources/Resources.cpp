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
searchPath( const std::string& pattern, const std::string& offset, void* libSymbol ) {
    std::string basePath = cpplocate::locatePath( pattern, offset, libSymbol );
    return std::filesystem::canonical( std::filesystem::path( basePath ) );
}

std::string getRadiumResourcesPath() {
    auto basePath =
        searchPath( "Resources/Shaders", "", reinterpret_cast<void*>( getRadiumResourcesPath ) );
    return ( basePath / "Resources" / "" ).string();
}

std::string getRadiumPluginsPath() {
    auto basePath =
        searchPath( "Plugins/lib", "", reinterpret_cast<void*>( getRadiumPluginsPath ) );
    return ( basePath / "Plugins" / "lib" / "" ).string();
}

std::string getBasePath() {
    return cpplocate::getModulePath();
}

std::string getBaseResourcesPath() {
    auto baseDir = searchPath( "Resources", "", nullptr );
    return ( baseDir / "Resources" / "" ).string();
}

std::string getResourcesPath( void* symbol, const std::string& offset ) {
    return ( searchPath( "/", "", symbol ) / offset ).string();
}

std::string getResourcesPath( void* symbol, std::string pattern, const std::string& offset ) {
    return ( searchPath( pattern, offset, symbol ) / pattern ).string();
}

} // namespace Resources
} // namespace Core
} // namespace Ra
