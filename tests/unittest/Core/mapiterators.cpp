#include <Core/Utils/StdMapIterators.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <utility>

TEST_CASE( "Core/Utils/StdMapIterators", "[unittests][Core][Core/Utils][StdMapIterators]" ) {
    std::map<int, std::string> myMap { { 2, "2" }, { 3, "3" } };

    //! [Iterating over keys]
    for ( const auto& k : Ra::Core::Utils::map_keys( myMap ) )
        REQUIRE( k == std::stoi( myMap[k] ) );
    //! [Iterating over keys]

    //! [Iterating over values]
    for ( auto& v : Ra::Core::Utils::map_values( myMap ) )
        v = std::string( "-" ) + v;
    //! [Iterating over values]

    for ( const auto& k : Ra::Core::Utils::map_keys( myMap ) )
        REQUIRE( k == -std::stoi( myMap[k] ) );
}
