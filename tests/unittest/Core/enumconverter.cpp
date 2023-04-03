#include <catch2/catch.hpp>

#include <Core/Utils/EnumConverter.hpp>

#include <random>

TEST_CASE( "Core/Utils/EnumConverter", "[Core][Core/Utils][EnumConverter]" ) {
    SECTION( "Test enum converter" ) {
        using namespace Ra::Core::Utils;

        enum Values : unsigned int { VALUE_0 = 10, VALUE_1 = 20, VALUE_2 = 30 };
        using ValuesType = typename std::underlying_type_t<Values>;

        Ra::Core::Utils::EnumConverter<ValuesType> myEnum { { Values::VALUE_0, "VALUE_0" },
                                                            { Values::VALUE_1, "VALUE_1" },
                                                            { Values::VALUE_2, "VALUE_2" } };

        REQUIRE( myEnum.getEnumerator( Values::VALUE_0 ) == "VALUE_0" );
        REQUIRE( myEnum.getEnumerator( "VALUE_0" ) == Values::VALUE_0 );
        REQUIRE( myEnum.getEnumerator( Values::VALUE_1 ) == "VALUE_1" );
        REQUIRE( myEnum.getEnumerator( "VALUE_1" ) == Values::VALUE_1 );
        REQUIRE( myEnum.getEnumerator( Values::VALUE_2 ) == "VALUE_2" );
        REQUIRE( myEnum.getEnumerator( "VALUE_2" ) == Values::VALUE_2 );
        auto strVect = myEnum.getEnumerators();
        REQUIRE( strVect.size() == 3 );
        REQUIRE( strVect[0] == "VALUE_0" );
        REQUIRE( strVect[1] == "VALUE_1" );
        REQUIRE( strVect[2] == "VALUE_2" );
    }
}
