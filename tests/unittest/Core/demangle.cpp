#include <Core/Utils/TypesUtils.hpp>
#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include <iostream>

TEST_CASE( "Core/Utils/TypesUtils", "[Core][Core/Utils][TypesUtils]" ) {
    SECTION( "Demangle from typename" ) {
        using Ra::Core::Utils::demangleType;

        REQUIRE( std::string( demangleType<int>() ) == "int" );
        REQUIRE( std::string( demangleType<float>() ) == "float" );
        REQUIRE( std::string( demangleType<uint>() ) == "unsigned int" );
        // TODO, verify type demangling on windows
#ifndef _WIN32
        REQUIRE( std::string( demangleType<size_t>() ) == "unsigned long" );

        auto demangledName = std::string( demangleType<std::vector<int>>() );
        REQUIRE( demangledName == "std::vector<int, std::allocator<int> >" );
#endif
    }

    SECTION( "Demangle from instance" ) {
        using Ra::Core::Utils::demangleType;

        int i { 1 };
        float f { 2 };
        unsigned int u { 3 };
        size_t s { 4 };

        REQUIRE( std::string( demangleType( i ) ) == "int" );
        REQUIRE( std::string( demangleType( f ) ) == "float" );
        REQUIRE( std::string( demangleType( u ) ) == "unsigned int" );
        // TODO, verify type demangling on windows
#ifndef _WIN32
        REQUIRE( std::string( demangleType<size_t>() ) == "unsigned long" );

        std::vector<int> v;
        auto demangledName = std::string( demangleType( v ) );
        REQUIRE( demangledName == "std::vector<int, std::allocator<int> >" );
#endif
    }
}
