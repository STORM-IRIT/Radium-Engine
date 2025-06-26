#include <Core/Utils/Singleton.hpp>

#include <catch2/catch_test_macros.hpp>

class TestSingleton
{
    RA_SINGLETON_INTERFACE( TestSingleton );

  public:
    static int cpt;

  private:
    TestSingleton() { cpt++; }
};

int TestSingleton::cpt = 0;

RA_SINGLETON_IMPLEMENTATION( TestSingleton );

TEST_CASE( "Core/Utils/Singleton", "[unittests][Core][Core/Utils][Singleton]" ) {

    REQUIRE( TestSingleton::getInstance() == nullptr );
    auto p1 = TestSingleton::createInstance();
    REQUIRE( TestSingleton::getInstance() != nullptr );
    REQUIRE( p1 == TestSingleton::getInstance() );
    REQUIRE( TestSingleton::cpt == 1 );

    TestSingleton::destroyInstance();
    REQUIRE( TestSingleton::getInstance() == nullptr );
    auto p2 = TestSingleton::createInstance();
    REQUIRE( TestSingleton::getInstance() != nullptr );
    REQUIRE( p2 == TestSingleton::getInstance() );
    REQUIRE( TestSingleton::cpt == 2 );
}
