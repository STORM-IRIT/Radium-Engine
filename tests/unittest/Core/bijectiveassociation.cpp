#include <Core/Utils/BijectiveAssociation.hpp>

#include <string>

#include <catch2/catch.hpp>

using namespace Ra::Core::Utils;

TEST_CASE( "Core/Utils/BijectiveAssociation", "[Core][Core/Utils][BijectiveAssociation]" ) {

    SECTION( "Bijection between different types" ) {
        BijectiveAssociation<std::string, int> myTranslator { { "One", 1 }, { "Two", 2 } };
        myTranslator.addAssociation( "Three", 3 );
        myTranslator.addAssociation( 4, "Four" );
        myTranslator.addAssociation( { "Five", 5 } );
        myTranslator.addAssociation( { 6, "Six" } );

        REQUIRE( myTranslator( "Four" ) == 4 );
        REQUIRE( myTranslator( 3 ) == "Three" );
        REQUIRE( myTranslator.inject( "One" ) == 1 );
        REQUIRE( myTranslator.surject( 2 ) == "Two" );
    }

    SECTION( "Bijection between same types" ) {
        BijectiveAssociation<std::string, std::string> myTranslator { { "One", "Un" },
                                                                      { "Two", "Deux" } };
        myTranslator.addAssociation( "Three", "Trois" );
        myTranslator.addAssociation( { "Four", "Quatre" } );
        REQUIRE( myTranslator( "Four" ) == "Quatre" );
        REQUIRE( myTranslator.inject( "Three" ) == "Trois" );
        REQUIRE( myTranslator.surject( "Deux" ) == "Two" );
        REQUIRE( myTranslator.inject( "Two" ) == "Deux" );
    }
}
