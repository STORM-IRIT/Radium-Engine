#include <Core/Utils/BijectiveAssociation.hpp>

#include <string>

#include <catch2/catch.hpp>

using namespace Ra::Core::Utils;

TEST_CASE( "Core/Utils/BijectiveAssociation", "[Core][Core/Utils][BijectiveAssociation]" ) {

    SECTION( "Bijection between different types" ) {
        BijectiveAssociation<std::string, int> myTranslator { { "One", 1 }, { "Two", 2 } };
        myTranslator.insert( "Three", 3 );
        myTranslator.insert( { "Four", 4 } );

        REQUIRE( myTranslator( "Four" ) == 4 );
        REQUIRE( myTranslator.key( 3 ) == "Three" );
        REQUIRE( myTranslator.value( "One" ) == 1 );
        REQUIRE( myTranslator.key( 2 ) == "Two" );

        auto res = myTranslator.insert( "Four", 2 );
        REQUIRE( !res );

        auto n = myTranslator.size();
        REQUIRE( n == 4 );
    }

    SECTION( "Bijection between same types" ) {
        BijectiveAssociation<std::string, std::string> myTranslator { { "One", "Un" },
                                                                      { "Two", "Deux" } };
        myTranslator.insert( "Three", "Trois" );
        REQUIRE( myTranslator.insert( { "Four", "Quatre" } ) ); // first insert pass
        REQUIRE( !myTranslator.insert(
            { "Four", "Quatre" } ) ); // seconid insert failed since already present

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
