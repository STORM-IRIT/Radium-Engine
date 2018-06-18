#include <Tests/Manager.hpp>
#include <Tests/Tests.hpp>

#include <algorithm>

namespace RaTests {

RA_SINGLETON_IMPLEMENTATION( TestManager );

void TestManager::add( Test* test ) {
    m_tests.push_back( TestEntry( test ) );
}

int TestManager::run() {
    for ( auto& t : m_tests )
    {
        t.m_test->run();
    }

    uint numFailed = 0;

    printf( "Tests summary : \n" );

    for ( uint numTest = 0; numTest < m_tests.size(); ++numTest )
    {
        const auto& t = m_tests[numTest];
        if ( t.m_fails > 0 )
        {
            printf( "\tTest %i FAILED (%i unit tests failed)\n", numTest, t.m_fails );
            numFailed++;
        }
        else
        { printf( "\tTest %i PASSED \n", numTest ); }
    }

    printf( "Result : %lu / %lu tests passed\n", ulong( m_tests.size() - numFailed ),
            ulong( m_tests.size() ) );
    return numFailed;
}

void TestManager::testFailed( const Test* test ) {
    auto t = std::find_if( m_tests.begin(), m_tests.end(),
                           [test]( const TestEntry& entry ) { return entry.m_test == test; } );

    CORE_ASSERT( t != m_tests.cend(), "Failing test not registred." );

    ++( t->m_fails );

    // The program will break here if you enabled the "break on failure" option.
    if ( m_options.m_breakOnFailure )
    {
        BREAKPOINT( 0 );
    }
}
} // namespace RaTests
