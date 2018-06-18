#ifndef RADIUM_STRINGTESTS_HPP_
#define RADIUM_STRINGTESTS_HPP_

#include <Core/String/StringUtils.hpp>
#include <Tests/Tests.hpp>

namespace RaTests {
class StringTests : public Test {
    void run() override {

        using Ra::Core::StringUtils::getBaseName;
        using Ra::Core::StringUtils::getDirName;
        using Ra::Core::StringUtils::getFileExt;

        // Test getFileExt
        {
            RA_UNIT_TEST( getFileExt( "aaa.xyz" ) == std::string( "xyz" ), "File extension" );
            RA_UNIT_TEST( getFileExt( "aaa/bbb.xyz" ) == std::string( "xyz" ),
                          "Extension of relative path" );
            RA_UNIT_TEST( getFileExt( "/aaa/bbb/ccc.xyz" ) == std::string( "xyz" ),
                          "Extension of absolute path" );
            RA_UNIT_TEST( getFileExt( "aaa/bbb/xyz" ) == std::string( "" ),
                          "File with no extension" );
            RA_UNIT_TEST( getFileExt( "aaa/bbb/xyz." ) == std::string( "" ),
                          "File with no extension" );
        }

        // Test getDirName
        {
            RA_UNIT_TEST( getDirName( "aaa.xyz" ) == std::string( "." ), "File with no directory" );
            RA_UNIT_TEST( getDirName( "aaa/bbb.xyz" ) == std::string( "aaa" ), "Relative path" );
            RA_UNIT_TEST( getDirName( "/aaa/bbb/ccc.xyz" ) == std::string( "/aaa/bbb" ),
                          "Absolute path" );
            RA_UNIT_TEST( getDirName( "/aaa/bbb/ccc.xyz///" ) == std::string( "/aaa/bbb" ),
                          "Trailing slashes" );
            RA_UNIT_TEST( getDirName( "aaa/bbb/xyz" ) == std::string( "aaa/bbb" ),
                          "File with no extension" );
        }

        // Test getBaseName
        {
            RA_UNIT_TEST( getBaseName( "aaa.xyz", true ) == std::string( "aaa.xyz" ),
                          "File with no directory" );
            RA_UNIT_TEST( getBaseName( "aaa.xyz", false ) == std::string( "aaa" ),
                          "File with no directory" );
            RA_UNIT_TEST( getBaseName( "aaa/bbb.xyz", true ) == std::string( "bbb.xyz" ),
                          "Relative path" );
            RA_UNIT_TEST( getBaseName( "aaa/bbb.xyz", false ) == std::string( "bbb" ),
                          "Relative path" );
            RA_UNIT_TEST( getBaseName( "/aaa/bbb/ccc.xyz", true ) == std::string( "ccc.xyz" ),
                          "Absolute path" );
            RA_UNIT_TEST( getBaseName( "/aaa/bbb/ccc.xyz", false ) == std::string( "ccc" ),
                          "Absolute path" );
            RA_UNIT_TEST( getBaseName( "/aaa/bbb/ccc.xyz///", false ) == std::string( "ccc" ),
                          "Trailing slashes" );
            RA_UNIT_TEST( getBaseName( "aaa/bbb/xyz", true ) == std::string( "xyz" ),
                          "File with no extension" );
            RA_UNIT_TEST( getBaseName( "aaa/bbb/xyz", false ) == std::string( "xyz" ),
                          "File with no extension" );
        }

        std::string path = "/aaa/bbb/ccc.xyz";
        RA_UNIT_TEST( getDirName( path ) + "/" + getBaseName( path, false ) + "." +
                              getFileExt( path ) ==
                          path,
                      "Path reconstruction" );

        // Test string printf
        using Ra::Core::StringUtils::stringPrintf;

        const char* format = "test %u test %p test %f";
        char buffer[100];
        memset( buffer, 0x0, 100 );
        std::string str;

        int v1 = sprintf( buffer, format, 42, this, 3.14 );
        int v2 = stringPrintf( str, format, 42, this, 3.14 );
        RA_UNIT_TEST( v1 == v2 && str == std::string( buffer ), "String printf" );
    }
};
RA_TEST_CLASS( StringTests );
} // namespace RaTests

#endif // RADIUM_ALGEBRATESTS_HPP_
