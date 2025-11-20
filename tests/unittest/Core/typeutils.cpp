#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/TypesUtils.hpp>
#include <catch2/catch_test_macros.hpp>

namespace TypeTests {
struct TypeName_struct {};

struct SimpleStruct {
    std::vector<std::string> strings;
    bool boolean;
};
using PairAlias = std::pair<std::string, bool>;

} // namespace TypeTests
TEST_CASE( "Core/Utils/TypesUtils", "[unittests][Core][Utils][TypesUtils]" ) {
    SECTION( "Demangle from typename" ) {
        using Ra::Core::Utils::demangleType;
        using Ra::Core::Utils::simplifiedDemangledType;

        REQUIRE( demangleType<int>() == "int" );
        REQUIRE( demangleType<float>() == "float" );
        REQUIRE( demangleType<uint>() == "unsigned int" );
        REQUIRE( demangleType<size_t>() == "unsigned long" );

        REQUIRE( demangleType( std::vector<int> {} ) == "std::vector<int, std::allocator<int>>" );
        REQUIRE( demangleType<std::vector<int>>() == "std::vector<int, std::allocator<int>>" );
        REQUIRE( demangleType( std::type_index( typeid( std::vector<int> ) ) ) ==
                 "std::vector<int, std::allocator<int>>" );

        REQUIRE( demangleType( std::vector<float> {} ) ==
                 "std::vector<float, std::allocator<float>>" );
        REQUIRE( demangleType<std::vector<float>>() ==
                 "std::vector<float, std::allocator<float>>" );
        REQUIRE( demangleType( std::type_index( typeid( std::vector<float> ) ) ) ==
                 "std::vector<float, std::allocator<float>>" );

        std::string string_vector_type =
            "std::vector<std::basic_string<char, std::char_traits<char>, "
            "std::allocator<char>>, std::allocator<std::basic_string<char, "
            "std::char_traits<char>, std::allocator<char>>>>";
        REQUIRE( demangleType( std::vector<std::string> {} ) == string_vector_type );
        REQUIRE( demangleType<std::vector<std::string>>() == string_vector_type );
        REQUIRE( demangleType( std::type_index( typeid( std::vector<std::string> ) ) ) ==
                 string_vector_type );
        std::string simplified_string_vector_type = "vector<string>";
        REQUIRE( simplifiedDemangledType( std::vector<std::string> {} ) ==
                 simplified_string_vector_type );
        REQUIRE( simplifiedDemangledType<std::vector<std::string>>() ==
                 simplified_string_vector_type );
        REQUIRE( simplifiedDemangledType( std::type_index( typeid( std::vector<std::string> ) ) ) ==
                 simplified_string_vector_type );

        REQUIRE( demangleType<std::unordered_map<std::string, TypeTests::SimpleStruct>>() ==
                 "std::unordered_map<std::basic_string<char, std::char_traits<char>, "
                 "std::allocator<char>>, TypeTests::SimpleStruct, "
                 "std::hash<std::basic_string<char, std::char_traits<char>, "
                 "std::allocator<char>>>, std::equal_to<std::basic_string<char, "
                 "std::char_traits<char>, std::allocator<char>>>, "
                 "std::allocator<std::pair<std::basic_string<char, "
                 "std::char_traits<char>, "
                 "std::allocator<char>> const, TypeTests::SimpleStruct>>>" );

        REQUIRE( demangleType<TypeTests::TypeName_struct>() == "TypeTests::TypeName_struct" );
        REQUIRE( demangleType<TypeTests::SimpleStruct>() == "TypeTests::SimpleStruct" );

        REQUIRE( demangleType<TypeTests::PairAlias>() ==
                 "std::pair<std::basic_string<char, std::char_traits<char>, "
                 "std::allocator<char>>, bool>" );
        REQUIRE( demangleType<std::vector<TypeTests::PairAlias>>() ==
                 "std::vector<std::pair<std::basic_string<char, std::char_traits"
                 "<char>, std::allocator<char>>, bool>, std::allocator<std::pair<std::"
                 "basic_string<char, std::char_traits<char>, std::allocator<char>>, bool>>>" );

        REQUIRE( simplifiedDemangledType<std::vector<float>>() == "vector<Scalar>" );
        REQUIRE(
            simplifiedDemangledType<std::unordered_map<std::string, TypeTests::SimpleStruct>>() ==
            "unordered_map<string, TypeTests::SimpleStruct>" );
        REQUIRE( simplifiedDemangledType<TypeTests::PairAlias>() == "pair<string, bool>" );
        REQUIRE( simplifiedDemangledType<std::vector<TypeTests::PairAlias>>() ==
                 "vector<pair<string, bool>>" );
    }

    SECTION( "Demangle from instance" ) {
        using Ra::Core::Utils::demangleType;

        int i { 1 };
        float f { 2 };
        unsigned int u { 3 };
        size_t s { 4 };

        REQUIRE( demangleType( i ) == "int" );
        REQUIRE( demangleType( f ) == "float" );
        REQUIRE( demangleType( u ) == "unsigned int" );
        REQUIRE( demangleType( s ) == "unsigned long" );

#ifndef _WIN32
        // this segfault on windows due to out_of_bound exception. why ???
        std::vector<int> v;
        auto demangledName = demangleType( v );
        REQUIRE( demangledName == "std::vector<int, std::allocator<int>>" );
#endif
        TypeTests::TypeName_struct tns;
        auto demangledNameFromStruct = demangleType( tns );
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
