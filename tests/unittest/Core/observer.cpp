#include <Core/Utils/Observable.hpp>
#include <catch2/catch.hpp>

using Ra::Core::Utils::Observable;

class ObservableTest : public Observable<>
{};

class ObservableTest2 : public Observable<int>
{};

class A
{
  public:
    void f() { m_a++; }
    void f2( int a ) { m_a = a; }
    static void g() { m_b++; }
    static void g2( int b ) { m_b = b; }

    int m_a {0};
    static int m_b;
};

int A::m_b = 0;

TEST_CASE( "Core/Utils/Observable", "[Core][Core/Utils][Observable]" ) {
    ObservableTest test;
    ObservableTest2 test2;

    A a;
    int c  = 0;
    A::m_b = 0;

    using Observer  = std::function<void( void )>;
    using Observer2 = std::function<void( int )>;

    auto bf = std::bind( &A::f, &a );

    Observer obf         = bf;
    auto& observerTarget = obf.target_type();
    // "Type failed."
    REQUIRE( obf.target_type() == observerTarget );

    auto gid = test.attach( A::g );
    test.attach( bf );
    test.attach( [&c]() { c++; } );
    test.notify();

    REQUIRE( c == 1 );
    REQUIRE( a.m_a == 1 );
    REQUIRE( A::m_b == 1 );

    test.detach( gid );
    test.notify();

    REQUIRE( c == 2 );
    REQUIRE( a.m_a == 2 );
    // Test detach
    REQUIRE( A::m_b == 1 );

    test.attach( A::g );
    test.attach( A::g );
    test.attach( bf );
    test.attach( [&c]() { c++; } );
    test.notify();

    REQUIRE( c == 4 );
    REQUIRE( a.m_a == 4 );
    REQUIRE( A::m_b == 3 );

    test.detachAll();
    test.notify();

    // Test detach
    REQUIRE( c == 4 );
    REQUIRE( a.m_a == 4 );
    REQUIRE( A::m_b == 3 );

    test2.attachMember( &a, &A::f2 );
    test2.attach( &A::g2 );
    test2.attach( [&c]( int pc ) { c = pc; } );

    test2.notify( 5 );

    REQUIRE( c == 5 );
    REQUIRE( a.m_a == 5 );
    REQUIRE( A::m_b == 5 );

    test2.notify( 6 );

    REQUIRE( c == 6 );
    REQUIRE( a.m_a == 6 );
    REQUIRE( A::m_b == 6 );

    ObservableTest2 test2copy;
    test2.copyObserversTo( test2copy );

    test2.detachAll();
    test2.notify( 7 );

    REQUIRE( c == 6 );
    REQUIRE( a.m_a == 6 );
    REQUIRE( A::m_b == 6 );

    test2copy.notify( 7 );

    REQUIRE( c == 7 );
    REQUIRE( a.m_a == 7 );
    REQUIRE( A::m_b == 7 );
}
