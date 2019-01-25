#ifndef _RADIUM_TESTS_H_
#define _RADIUM_TESTS_H_

#include <Core/CoreMacros.hpp>
#include <Core/Utils/StdOptional.hpp>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>

#define DEFAULT_REPEAT 10

#define RADIUM_PP_MAKE_STRING2( S ) #S
#define RADIUM_PP_MAKE_STRING( S ) RADIUM_PP_MAKE_STRING2( S )

namespace Ra {
namespace Testing {

// A non-copyable move semantics struct
struct NonCopy {
    NonCopy( int x ) : value( x ) {}
    NonCopy( NonCopy&& other ) {
        value = other.value;
        other.value = 0;
    }
    NonCopy& operator=( NonCopy&& other ) {
        value = other.value;
        other.value = 0;
        return *this;
    }

    int value;

  private:
    NonCopy( const NonCopy& ) = delete;
    NonCopy& operator=( const NonCopy& ) = delete;
};

static std::vector<std::string> g_test_stack;
static int g_repeat;
static unsigned int g_seed;
static bool g_has_set_repeat, g_has_set_seed;

void verify_impl( bool condition, const char* testname, const char* file, int line,
                  const char* condition_as_string ) {
    if ( !condition )
    {
        std::cerr << "Test " << testname << " failed in " << file << " (" << line << ")"
                  << std::endl
                  << "    " << condition_as_string << std::endl;
        abort();
    }
}

#ifdef have_optional
template <typename S>
void verify_impl( const Core::Utils::optional<S>& condition, const char* testname, const char* file,
                  int line, const char* condition_as_string ) {
    if ( !condition )
    {
        std::cerr << "Test " << testname << " failed in " << file << " (" << line << ")"
                  << std::endl
                  << "    " << condition_as_string << std::endl;
        abort();
    }
}

template <typename S>
inline bool isApprox( const Core::Utils::optional<S>& lhs, const S& rhs ) {
    return lhs.has_value() && almost_equals( *lhs, rhs );
}

template <typename S>
inline bool isApprox( const S& lhs, const Core::Utils::optional<S>& rhs ) {
    return rhs.has_value() && almost_equals( lhs, *rhs );
}
#endif

template <typename S>
inline bool isApprox( const S& lhs, const S& rhs ) {
    return almost_equals( lhs, rhs );
}

#define RA_VERIFY( a, DESC )                                                                   \
    Ra::Testing::verify_impl( a, Ra::Testing::g_test_stack.back().c_str(), __FILE__, __LINE__, \
                              DESC ) // RADIUM_PP_MAKE_STRING(a))

#if defined( _MSC_VER ) && !defined( __INTEL_COMPILER )
#    define MYPRAGMA( X ) __pragma( X )
#else
#    define MYPRAGMA( X ) _Pragma( X )
#endif

#define CALL_SUBTEST( FUNC )                                                      \
    do                                                                            \
    {                                                                             \
        MYPRAGMA( "omp critical" ) {                                              \
            Ra::Testing::g_test_stack.push_back( RADIUM_PP_MAKE_STRING( FUNC ) ); \
        }                                                                         \
        FUNC;                                                                     \
        MYPRAGMA( "omp critical" ) { Ra::Testing::g_test_stack.pop_back(); }      \
    } while ( 0 )

inline void set_repeat_from_string( const char* str ) {
    errno = 0;
    g_repeat = int( strtoul( str, 0, 10 ) );
    if ( errno || g_repeat <= 0 )
    {
        std::cout << "Invalid repeat value " << str << std::endl;
        exit( EXIT_FAILURE );
    }
    g_has_set_repeat = true;
}

inline void set_seed_from_string( const char* str ) {
    errno = 0;
    g_seed = int( strtoul( str, 0, 10 ) );
    if ( errno || g_seed == 0 )
    {
        std::cout << "Invalid seed value " << str << std::endl;
        exit( EXIT_FAILURE );
    }
    g_has_set_seed = true;
}

static bool init_testing( int argc, const char* argv[] ) {
    g_has_set_repeat = false;
    g_has_set_seed = false;
    bool need_help = false;

    for ( int i = 1; i < argc; i++ )
    {
        if ( argv[i][0] == 'r' )
        {
            if ( g_has_set_repeat )
            {
                std::cout << "Argument " << argv[i] << " conflicting with a former argument"
                          << std::endl;
                return 1;
            }
            set_repeat_from_string( argv[i] + 1 );
        } else if ( argv[i][0] == 's' )
        {
            if ( g_has_set_seed )
            {
                std::cout << "Argument " << argv[i] << " conflicting with a former argument"
                          << std::endl;
                return false;
            }
            set_seed_from_string( argv[i] + 1 );
        } else
        { need_help = true; }
    }

    if ( need_help )
    {
        std::cout << "This test application takes the following optional arguments:" << std::endl;
        std::cout << "  rN     Repeat each test N times (default: " << DEFAULT_REPEAT << ")"
                  << std::endl;
        std::cout << "  sN     Use N as seed for random numbers (default: based on current time)"
                  << std::endl;
        std::cout << std::endl;
        std::cout << "If defined, the environment variables RADIUM_REPEAT and RADIUM_SEED"
                  << std::endl;
        std::cout << "will be used as default values for these parameters." << std::endl;
        return false;
    }

    char* env_RADIUM_REPEAT = getenv( "RADIUM_REPEAT" );
    if ( !g_has_set_repeat && env_RADIUM_REPEAT )
        set_repeat_from_string( env_RADIUM_REPEAT );
    char* env_RADIUM_SEED = getenv( "RADIUM_SEED" );
    if ( !g_has_set_seed && env_RADIUM_SEED )
        set_seed_from_string( env_RADIUM_SEED );

    if ( !g_has_set_seed )
        g_seed = (unsigned int)time( NULL );
    if ( !g_has_set_repeat )
        g_repeat = DEFAULT_REPEAT;

    std::cout << "Initializing random number generator with seed " << g_seed << std::endl;
    std::stringstream ss;
    ss << "Seed: " << g_seed;
    g_test_stack.push_back( ss.str() );
    srand( g_seed );
    std::cout << "Repeating each test " << g_repeat << " times" << std::endl;

    return true;
}

} // namespace Testing
} // namespace Ra

#endif // _RADIUM_TESTS_H_
