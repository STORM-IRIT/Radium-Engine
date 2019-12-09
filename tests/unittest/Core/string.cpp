#include <Core/Utils/StringUtils.hpp>
#include <catch2/catch.hpp>


TEST_CASE( "Core/Utils/StringUtils", "[Core][Core/Utils][StringUtils]" )
{
    SECTION( "Test getFileExt" )
    {
        using Ra::Core::Utils::getFileExt;
        // File extension
        REQUIRE( getFileExt( "aaa.xyz" ) == std::string( "xyz" ) );
        // Extension of relative path
        REQUIRE( getFileExt( "aaa/bbb.xyz" ) == std::string( "xyz" ) );
        // Extension of absolute path
        REQUIRE( getFileExt( "/aaa/bbb/ccc.xyz" ) == std::string( "xyz" ) );
        // File with no extension
        REQUIRE( getFileExt( "aaa/bbb/xyz" ) == std::string( "" ) );
        REQUIRE( getFileExt( "aaa/bbb/xyz." ) == std::string( "" ) );
    }
    SECTION( "Test getDirName" )
    {
        using Ra::Core::Utils::getDirName;
        // File with no directory
        REQUIRE( getDirName( "aaa.xyz" ) == std::string( "." ) );
        // Relative path
        REQUIRE( getDirName( "aaa/bbb.xyz" ) == std::string( "aaa" ) );
         // Absolute path
        REQUIRE( getDirName( "/aaa/bbb/ccc.xyz" ) == std::string( "/aaa/bbb" ) );
         // Trailing slashes
        REQUIRE( getDirName( "/aaa/bbb/ccc.xyz///" ) == std::string( "/aaa/bbb" ) );
        // File with no extension
        REQUIRE( getDirName( "aaa/bbb/xyz" ) == std::string( "aaa/bbb" ) );
    }
    SECTION( "Test getBaseName" )
    {
        using Ra::Core::Utils::getBaseName;
        // File with no directory
        REQUIRE( getBaseName( "aaa.xyz", true ) == std::string( "aaa.xyz" ) );
        REQUIRE( getBaseName( "aaa.xyz", false ) == std::string( "aaa" ) );
        // Relative path
        REQUIRE( getBaseName( "aaa/bbb.xyz", true ) == std::string( "bbb.xyz" ) );
        REQUIRE( getBaseName( "aaa/bbb.xyz", false ) == std::string( "bbb" ) );
        // Absolute path
        REQUIRE( getBaseName( "/aaa/bbb/ccc.xyz", true ) == std::string( "ccc.xyz" ) );
        REQUIRE( getBaseName( "/aaa/bbb/ccc.xyz", false ) == std::string( "ccc" ) );
        // Trailing slashes
        REQUIRE( getBaseName( "/aaa/bbb/ccc.xyz///", false ) == std::string( "ccc" ) );
        // File with no extension
        REQUIRE( getBaseName( "aaa/bbb/xyz", true ) == std::string( "xyz" ) );
        REQUIRE( getBaseName( "aaa/bbb/xyz", false ) == std::string( "xyz" ) );
    }
    SECTION( "Test jointly getDirName, getBaseName and getFileExt")
    {
        using Ra::Core::Utils::getFileExt;
        using Ra::Core::Utils::getDirName;
        using Ra::Core::Utils::getBaseName;
        // Path reconstruction
        std::string path = "/aaa/bbb/ccc.xyz";
        REQUIRE( getDirName( path ) + "/" + getBaseName( path, false ) + "." +
                              getFileExt( path ) == path );

    }
} // TEST_CASE




