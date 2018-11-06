#include <Core/String/StringUtils.hpp>

#include <cstring> //memset
#include <Tests.hpp>


namespace Ra {
namespace Testing {

void run()  {

    using Ra::Core::StringUtils::getBaseName;
    using Ra::Core::StringUtils::getDirName;
    using Ra::Core::StringUtils::getFileExt;

    // Test getFileExt
    {
        RA_VERIFY( getFileExt( "aaa.xyz" ) == std::string( "xyz" ), "File extension" );
        RA_VERIFY( getFileExt( "aaa/bbb.xyz" ) == std::string( "xyz" ),

                      "Extension of relative path" );
        RA_VERIFY( getFileExt( "/aaa/bbb/ccc.xyz" ) == std::string( "xyz" ),
                      "Extension of absolute path" );
        RA_VERIFY( getFileExt( "aaa/bbb/xyz" ) == std::string( "" ),
                      "File with no extension" );
        RA_VERIFY( getFileExt( "aaa/bbb/xyz." ) == std::string( "" ),
                      "File with no extension" );

    }

    // Test getDirName
    {
        RA_VERIFY( getDirName( "aaa.xyz" ) == std::string( "." ), "File with no directory" );
        RA_VERIFY( getDirName( "aaa/bbb.xyz" ) == std::string( "aaa" ), "Relative path" );

        RA_VERIFY( getDirName( "/aaa/bbb/ccc.xyz" ) == std::string( "/aaa/bbb" ),
                      "Absolute path" );
        RA_VERIFY( getDirName( "/aaa/bbb/ccc.xyz///" ) == std::string( "/aaa/bbb" ),
                      "Trailing slashes" );
        RA_VERIFY( getDirName( "aaa/bbb/xyz" ) == std::string( "aaa/bbb" ),
                      "File with no extension" );

    }

    // Test getBaseName
    {
        RA_VERIFY( getBaseName( "aaa.xyz", true ) == std::string( "aaa.xyz" ),

                      "File with no directory" );
        RA_VERIFY( getBaseName( "aaa.xyz", false ) == std::string( "aaa" ),
                      "File with no directory" );
        RA_VERIFY( getBaseName( "aaa/bbb.xyz", true ) == std::string( "bbb.xyz" ),
                      "Relative path" );
        RA_VERIFY( getBaseName( "aaa/bbb.xyz", false ) == std::string( "bbb" ),
                      "Relative path" );
        RA_VERIFY( getBaseName( "/aaa/bbb/ccc.xyz", true ) == std::string( "ccc.xyz" ),
                      "Absolute path" );
        RA_VERIFY( getBaseName( "/aaa/bbb/ccc.xyz", false ) == std::string( "ccc" ),
                      "Absolute path" );
        RA_VERIFY( getBaseName( "/aaa/bbb/ccc.xyz///", false ) == std::string( "ccc" ),
                      "Trailing slashes" );
        RA_VERIFY( getBaseName( "aaa/bbb/xyz", true ) == std::string( "xyz" ),
                      "File with no extension" );
        RA_VERIFY( getBaseName( "aaa/bbb/xyz", false ) == std::string( "xyz" ),
                      "File with no extension" );
    }

    std::string path = "/aaa/bbb/ccc.xyz";
    RA_VERIFY( getDirName( path ) + "/" + getBaseName( path, false ) + "." +
                          getFileExt( path ) ==
                      path,
                  "Path reconstruction" );

//    // Test string printf
//    using Ra::Core::StringUtils::stringPrintf;

//    const char* format = "test %u test %p test %f";
//    char buffer[100];
//    std::memset( buffer, 0x0, 100 );
//    std::string str;

//    int v1 = sprintf( buffer, format, 42, 3.14 );
//    int v2 = stringPrintf( str, format, 42, 3.14 );
//    RA_VERIFY( v1 == v2, "String printf 1" );
//    RA_VERIFY( str == std::string( buffer ), "String printf 2" );

}
} // namespace Testing
} // namespace Ra


int main(int argc, const char **argv) {
    using namespace Ra;

    if(!Testing::init_testing(1, argv))

    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for

    for(int i = 0; i < Testing::g_repeat; ++i)
    {
        CALL_SUBTEST(( Testing::run() ));
    }

    return EXIT_SUCCESS;
}
