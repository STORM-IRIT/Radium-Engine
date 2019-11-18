#include <Core/Resources/Resources.hpp>

#include <cpplocate/cpplocate.h>

namespace Ra {
namespace Core {
namespace Resources {
std::string getRadiumResourcesDir() {
    auto libraryPath =
        cpplocate::getLibraryPath( reinterpret_cast<void*>( getRadiumResourcesDir ) );
    auto libraryDir  = libraryPath.substr( 0, libraryPath.find_last_of( '/' ) );
    auto baseDir     = libraryDir.substr( 0, libraryDir.find_last_of( '/' ) + 1 );

    return baseDir + "Resources/";
}

std::string getRadiumPluginsDir() {
    auto libraryPath = cpplocate::getLibraryPath( reinterpret_cast<void*>( getRadiumPluginsDir ) );
    auto libraryDir  = libraryPath.substr( 0, libraryPath.find_last_of( '/' ) );
    auto baseDir     = libraryDir.substr( 0, libraryDir.find_last_of( '/' ) + 1 );

    return baseDir + "Plugins/lib/";
}

std::string getBaseDir() {
    return cpplocate::getModulePath();
}

ResourcesLocator::ResourcesLocator( void* symbol, const std::string& offset ) : m_basePath{""} {
    auto libraryPath = cpplocate::getLibraryPath( symbol );
    auto libraryDir  = libraryPath.substr( 0, libraryPath.find_last_of( '/' ) );
    m_basePath       = libraryDir + offset;
}
const std::string& ResourcesLocator::getBasePath() {
    return m_basePath;
}
} // namespace Resources
} // namespace Core
} // namespace Ra
