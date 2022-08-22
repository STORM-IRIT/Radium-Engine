#include <Core/Utils/BijectiveAssociation.hpp>

#include <string>

#include <catch2/catch.hpp>

using namespace Ra::Core::Utils;

TEST_CASE( "Core/Utils/BijectiveAssociation", "[Core][Core/Utils][BijectiveAssociation]" ) {

    SECTION( "Bijection between different types" ) {
        BijectiveAssociation<std::string, int> myTranslator { { "One", 1 }, { "Two", 2 } };
        myTranslator.addAssociation( "Three", 3 );
        myTranslator.addAssociation( { "Four", 4 } );

        REQUIRE( myTranslator( "Four" ) == 4 );
        REQUIRE( myTranslator.key( 3 ) == "Three" );
        REQUIRE( myTranslator.value( "One" ) == 1 );
        REQUIRE( myTranslator.key( 2 ) == "Two" );
    }

    SECTION( "Bijection between same types" ) {
        BijectiveAssociation<std::string, std::string> myTranslator { { "One", "Un" },
                                                                      { "Two", "Deux" } };
        myTranslator.addAssociation( "Three", "Trois" );
        myTranslator.addAssociation( { "Four", "Quatre" } );

        REQUIRE( myTranslator( "Four" ) == "Quatre" );
        REQUIRE( myTranslator.value( "Three" ) == "Trois" );
        REQUIRE( myTranslator.key( "Deux" ) == "Two" );
    }

    SECTION( "Iterators on Bijective association" ) {
        BijectiveAssociation<std::string, std::string> myTranslator {
            { "One", "Un" }, { "Two", "Deux" }, { "Three", "Trois" } };
        for ( const auto& e : myTranslator ) {
            REQUIRE( myTranslator.value( e.first ) == e.second );
            REQUIRE( myTranslator.key( e.second ) == e.first );
        }
    }
}
