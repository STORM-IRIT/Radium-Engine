#include <Core/Resources/Resources.hpp>

#include <Core/Utils/Log.hpp>
#include <algorithm>
#include <cpplocate/cpplocate.h>

namespace Ra {
namespace Core {
namespace Resources {

std::string searchPath( std::string pattern, std::string offset, void* libSymbol ) {
    std::string baseDir = cpplocate::locatePath( pattern, offset, libSymbol );
    std::replace( baseDir.begin(), baseDir.end(), '\\', '/' );
    int nup = 0;
    while ( baseDir.rfind( "../", baseDir.size() - 1 - 3 * nup ) != std::string::npos )
    {
        ++nup;
    }
    baseDir = baseDir.substr( 0, baseDir.size() - 3 * nup - 1 );
    while ( nup-- )
    {
        baseDir = baseDir.substr( 0, baseDir.find_last_of( '/' ) + 1 );
    }
    return baseDir;
}

std::string getRadiumResourcesDir() {
    const std::string baseDir = searchPath(
        "Resources/Shaders", "../..", reinterpret_cast<void*>( getRadiumResourcesDir ) );
    return baseDir + "Resources/";
}

std::string getRadiumPluginsDir() {
    const std::string baseDir =
        searchPath( "Plugins/lib", "../../..", reinterpret_cast<void*>( getRadiumPluginsDir ) );
    return baseDir + "Plugins/lib/";
}

std::string getBaseDir() {
    return cpplocate::getModulePath();
}

ResourcesLocator::ResourcesLocator( void* symbol, const std::string& offset ) : m_basePath{""} {
    m_basePath = searchPath( "/", "", symbol ) + offset;
}
const std::string& ResourcesLocator::getBasePath() {
    return m_basePath;
}
} // namespace Resources
} // namespace Core
} // namespace Ra
