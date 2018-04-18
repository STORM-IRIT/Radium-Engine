#ifndef RADIUM_TESTS_MANAGER_HPP_
#define RADIUM_TESTS_MANAGER_HPP_
#include <Core/Utils/Singleton.hpp>

#include <vector>

namespace RaTests {

class Test;

/// Singleton class responsible for running the tests.
class TestManager {

    RA_SINGLETON_INTERFACE( TestManager );

  public:
    /// Options regarding the test behavior.
    struct Options {
        Options() : m_breakOnFailure( false ) {}
        bool m_breakOnFailure;
    };

    /// One test instance and the number of times it has failed.
    struct TestEntry {
        TestEntry( Test* test ) : m_test( test ), m_fails( 0 ) {}
        Test* m_test;
        uint m_fails;
    };

    /// Empty constructor.
    TestManager() {}

    /// Register one test into the manager.
    void add( Test* test );

    /// Run all tests. Returns the number of tests instances that failed.
    int run();

    /// Function that a test calls when it fails.
    void testFailed( const Test* test );

  public:
    Options m_options;              /// Options of the tests.
    std::vector<TestEntry> m_tests; /// Storage for the test instances.
};

} // namespace RaTests

#endif // RADIUM_TESTS_MANAGER_HPP_
