#include <Core/Resources/Resources.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Resources", "[Core]" ) {
    using namespace Ra::Core::Resources;
    std::string radiumResourcesPath = getRadiumResourcesPath();
    std::string radiumPluginPath    = getRadiumPluginsPath();
    std::string basePath            = getBasePath();
    std::string baseResourcesPath   = getBaseResourcesPath();
    std::cout << radiumResourcesPath << "\n"
              << radiumPluginPath << "\n"
              << basePath << "\n"
              << baseResourcesPath << "\n";
    //    auto resourcesPath1 = getResourcesPath( void* symbol, const std::string& offset );
    //    auto resourcePath2 =
    //        getResourcesPath( void* symbol, std::string pattern, const std::string& offset );
}
