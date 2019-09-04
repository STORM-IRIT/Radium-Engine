#include <Core/Utils/Observable.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {

using Ra::Core::Utils::Observable;

class ObservableTest : public Observable<>
{};

class A
{
  public:
    void f() { m_a++; }
    static void g() { m_b++; }

    int m_a{0};
    static int m_b;
};

int A::m_b = 0;

void run() {
    ObservableTest test;
    A a;
    int c  = 0;
    A::m_b = 0;

    using Observer = std::function<void( void )>;

    auto bf = std::bind( &A::f, &a );

    Observer obf         = bf;
    auto& observerTarget = obf.target_type();
    RA_VERIFY( obf.target_type() == observerTarget, "Type failed." );

    auto gid = test.attach( A::g );
    test.attach( bf );
    test.attach( [&c]() { c++; } );
    test.notify();

    RA_VERIFY( c == 1, "Notify failed." );
    RA_VERIFY( a.m_a == 1, "Notify failed." );
    RA_VERIFY( A::m_b == 1, "Notify failed." );

    test.detach( gid );
    test.notify();

    RA_VERIFY( c == 2, "Notify failed." );
    RA_VERIFY( a.m_a == 2, "Notify failed." );
    RA_VERIFY( A::m_b == 1, "Notify failed." );

    test.attach( A::g );
    test.attach( A::g );
    test.attach( bf );
    test.attach( [&c]() { c++; } );
    test.notify();

    RA_VERIFY( c == 4, "Notify failed." );
    RA_VERIFY( a.m_a == 4, "Notify failed." );
    RA_VERIFY( A::m_b == 3, "Notify failed." );
    test.detachAll();
    test.notify();

    RA_VERIFY( c == 4, "Notify failed." );
    RA_VERIFY( a.m_a == 4, "Notify failed." );
    RA_VERIFY( A::m_b == 3, "Notify failed." );
}

} // namespace Testing
} // namespace Ra

int main( int argc, const char** argv ) {
    using namespace Ra;

    if ( !Testing::init_testing( 1, argv ) ) { return EXIT_FAILURE; }
    for ( int i = 0; i < Testing::g_repeat; ++i )
    {
        CALL_SUBTEST( ( Testing::run() ) );
    }

    return EXIT_SUCCESS;
}
