#include <Core/Resources/Resources.hpp>
#include <catch2/catch.hpp>

int dummy() {
    return 42;
}
TEST_CASE( "Core/Resources", "[Core]" ) {
    using namespace Ra::Core::Resources;
    auto radiumResourcesPath = getRadiumResourcesPath();
    auto radiumPluginPath    = getRadiumPluginsPath();
    auto basePath            = getBasePath();
    auto baseResourcesPath   = getResourcesPath();
    auto baseResourcesPath1 =
        getResourcesPath( reinterpret_cast<void*>( getRadiumResourcesPath ), "Resources" );
    auto baseResourcesPath2 = getResourcesPath( reinterpret_cast<void*>( dummy ), "" );
    auto baseResourcesPath3 = getResourcesPath( nullptr, "" );
    auto baseResourcesPath4 = getResourcesPath( nullptr, "hopeYouDontHaveAPathWithThisName" );

    std::cout << radiumResourcesPath.value_or( "not found" ) << "\n"
              << radiumPluginPath.value_or( "not found" ) << "\n"
              << basePath.value_or( "not found" ) << "\n"
              << baseResourcesPath.value_or( "not found" ) << "\n"
              << baseResourcesPath1.value_or( "not found" ) << "\n"
              << baseResourcesPath2.value_or( "not found" ) << "\n";

    REQUIRE( baseResourcesPath1 == radiumResourcesPath );
    REQUIRE( baseResourcesPath2 == basePath );
    REQUIRE( baseResourcesPath2 == baseResourcesPath3 );
    REQUIRE( !baseResourcesPath4 );
}
