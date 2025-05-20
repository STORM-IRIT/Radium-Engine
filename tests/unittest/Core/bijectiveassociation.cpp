#include <Core/Utils/BijectiveAssociation.hpp>
#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <utility>

using namespace Ra::Core::Utils;

TEST_CASE( "Core/Utils/BijectiveAssociation",
           "[unittests][Core][Core/Utils][BijectiveAssociation]" ) {

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
            { "Four", "Quatre" } ) ); // second insert failed since already present

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

    SECTION( "Erase, replace, get undef." ) {
        BijectiveAssociation<std::string, int> myTranslator {
            { "Foo", 1 }, { "Bar", 2 }, { "Baz", 3 } };

        REQUIRE_THROWS_AS( myTranslator.value( "Faz" ), std::out_of_range );
        REQUIRE_THROWS_AS( myTranslator.key( 4 ), std::out_of_range );

        REQUIRE( !myTranslator.valueIfExists( "Faz" ) );
        REQUIRE( !myTranslator.keyIfExists( 4 ) );

        REQUIRE( myTranslator.value( "Foo" ) == 1 );
        REQUIRE( myTranslator.key( 1 ) == "Foo" );
        myTranslator.replace( "Foo", 4 );
        REQUIRE( myTranslator.value( "Foo" ) == 4 );
        REQUIRE( myTranslator.key( 4 ) == "Foo" );

        REQUIRE( myTranslator.remove( "Bar", 2 ) );
        REQUIRE( !myTranslator.remove( "Unknown", 6 ) );
        REQUIRE( myTranslator.insert( "Bar", 6 ) );
        REQUIRE( myTranslator.value( "Bar" ) == 6 );
        REQUIRE( myTranslator.key( 6 ) == "Bar" );

        REQUIRE( myTranslator.value( "Foo" ) == 4 );
        REQUIRE( myTranslator.key( 4 ) == "Foo" );
        myTranslator.replace( "Faz", 4 );
        REQUIRE( myTranslator.value( "Faz" ) == 4 );
        REQUIRE( myTranslator.key( 4 ) == "Faz" );
    }
}
