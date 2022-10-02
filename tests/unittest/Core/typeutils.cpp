#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/TypesUtils.hpp>
#include <catch2/catch.hpp>

#include <string>
#include <vector>

#include <iostream>

namespace TypeTests {
struct TypeName_struct {};
} // namespace TypeTests
TEST_CASE( "Core/Utils/TypesUtils", "[Core][Utils][TypesUtils]" ) {
    SECTION( "Demangle from typename" ) {
        using Ra::Core::Utils::demangleType;

        REQUIRE( std::string( demangleType<int>() ) == "int" );
        REQUIRE( std::string( demangleType<float>() ) == "float" );
        REQUIRE( std::string( demangleType<uint>() ) == "unsigned int" );
        REQUIRE( std::string( demangleType<size_t>() ) == "unsigned long" );

        auto demangledName = std::string( demangleType<std::vector<int>>() );
        REQUIRE( demangledName == "std::vector<int, std::allocator<int>>" );

        demangledName = std::string( demangleType<TypeTests::TypeName_struct>() );
        REQUIRE( demangledName == "TypeTests::TypeName_struct" );
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
        REQUIRE( std::string( demangleType( s ) ) == "unsigned long" );

#ifndef _WIN32
        // this segfault on windows due to out_of_bound exception. why ???
        std::vector<int> v;
        auto demangledName = std::string( demangleType( v ) );
        REQUIRE( demangledName == "std::vector<int, std::allocator<int>>" );
#endif
        TypeTests::TypeName_struct tns;
        auto demangledNameFromStruct = std::string( demangleType( tns ) );
        REQUIRE( demangledNameFromStruct == "TypeTests::TypeName_struct" );
    }

    SECTION( "Type traits" ) {
        using namespace Ra::Core::Utils;
        REQUIRE( is_container<Scalar>::value == false );
        REQUIRE( is_container<Ra::Core::Vector3>::value == false );
        REQUIRE( is_container<Ra::Core::Utils::Color>::value == false );
        REQUIRE( is_container<Ra::Core::VectorArray<Ra::Core::Vector3>>::value == true );
        REQUIRE( is_container<std::array<unsigned int, 2>>::value == true );
        REQUIRE( is_container<std::map<size_t, std::string>>::value == true );
        REQUIRE( is_container<std::vector<Scalar>>::value == true );
    }
}
