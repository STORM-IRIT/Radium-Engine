#include <Core/Resources/Resources.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Core/Utils/StdFilesystem.hpp>
namespace fs = ::std::filesystem;

int dummy() {
    return 42;
}
TEST_CASE( "Core/Resources", "[unittests][Core]" ) {
    using namespace Ra::Core::Resources;
    auto radiumResourcesPath = getRadiumResourcesPath();
    auto radiumPluginPath    = getRadiumPluginsPath();
    auto basePath            = getBasePath();
    auto baseResourcesPath   = getResourcesPath();
    auto baseResourcesPath1 =
        getResourcesPath( reinterpret_cast<void*>( getRadiumResourcesPath ), "Resources/Radium" );
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

    auto defaultDataPath = getDataPath();
    REQUIRE( defaultDataPath == fs::current_path().string() );

    pushDataPath( "data/tmp/foo/" );
    pushDataPath( "data/tmp/bar/" );
    auto currentDataPath = getDataPath();
    REQUIRE( currentDataPath == "data/tmp/bar/" );
    currentDataPath = popDataPath();
    REQUIRE( currentDataPath == "data/tmp/bar/" );
    currentDataPath = getDataPath();
    REQUIRE( currentDataPath == "data/tmp/foo/" );
    popDataPath();
    currentDataPath = getDataPath();
    REQUIRE( currentDataPath == fs::current_path().string() );
    fs::remove( "data/tmp/foo/" );
    fs::remove( "data/tmp/bar/" );
    fs::remove( "data/tmp/" );
}
