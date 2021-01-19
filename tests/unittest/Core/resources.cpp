#include <Core/Resources/Resources.hpp>
#include <catch2/catch.hpp>

int dummy() {
    return 42;
}
TEST_CASE( "Core/Resources", "[Core]" ) {
    using namespace Ra::Core::Resources;
    std::string radiumResourcesPath = getRadiumResourcesPath();
    std::string radiumPluginPath    = getRadiumPluginsPath();
    std::string basePath            = getBasePath();
    std::string baseResourcesPath   = getResourcesPath();
    std::string baseResourcesPath1 =
        getResourcesPath( reinterpret_cast<void*>( getRadiumResourcesPath ), "Resources" );
    std::string baseResourcesPath2 = getResourcesPath( reinterpret_cast<void*>( dummy ), "" );
    std::string baseResourcesPath3 = getResourcesPath( nullptr, "" );
    std::string baseResourcesPath4 =
        getResourcesPath( nullptr, "hopeYouDontHaveAPathWithThisName" );

    std::cout << radiumResourcesPath << "\n"
              << radiumPluginPath << "\n"
              << basePath << "\n"
              << baseResourcesPath << "\n"
              << baseResourcesPath1 << "\n"
              << baseResourcesPath2 << "\n";

    REQUIRE( baseResourcesPath1 == radiumResourcesPath );
    REQUIRE( baseResourcesPath2 == basePath );
    REQUIRE( baseResourcesPath2 == baseResourcesPath3 );
    REQUIRE( "" == baseResourcesPath4 );

    //    auto resourcesPath1 = getResourcesPath( void* symbol, const std::string& offset );
    //    auto resourcePath2 =
    //        getResourcesPath( void* symbol, std::string pattern, const std::string& offset );
}
