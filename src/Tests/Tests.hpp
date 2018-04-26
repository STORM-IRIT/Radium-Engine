#ifndef RADIUM_TESTS_HPP_
#define RADIUM_TESTS_HPP_
#include <Core/CoreMacros.hpp>
#include <Tests/Manager.hpp>

namespace RaTests {
/// Base class for all tests.
class Test {
  public:
    Test() {
        if ( !TestManager::getInstance() )
        {
            TestManager::createInstance();
        }
        TestManager::getInstance()->add( this );
    }
    virtual void run() = 0;

    virtual ~Test(){};
};

// Poor man's singleton to automatically instantiate a test.
#define RA_TEST_CLASS( TYPE ) \
    namespace TYPE##NS {      \
        TYPE test_instance;   \
    }

/// This macro is similar to "CORE_ASSERT" but will print a message if
/// the test condition is false and signal it to the test manager.
#define RA_UNIT_TEST( EXP, DESC )                                                                 \
    MACRO_START                                                                                   \
    if ( !( EXP ) )                                                                               \
    {                                                                                             \
        fprintf( stderr, "[TEST FAILED] : %s:%i: `%s` : %s \n", __FILE__, __LINE__, #EXP, DESC ); \
        RaTests::TestManager::getInstance()->testFailed( this );                                  \
    }                                                                                             \
    else                                                                                          \
    { fprintf( stdout, "[TEST PASSED]\n" ); }                                                     \
    MACRO_END

/// A test that always pass.
class DummyTestPass : public Test {
    virtual void run() override { RA_UNIT_TEST( true, "Dummy test pass." ); }
};

/// A test that always fails.
class DummyTestFail : public Test {
    virtual void run() override { RA_UNIT_TEST( false, "Dummy test fail." ); }
};

} // namespace RaTests
#endif // RADIUM_TESTS_HPP_
