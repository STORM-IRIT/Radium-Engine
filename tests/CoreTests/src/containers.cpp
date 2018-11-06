#include <Core/Containers/Iterators.hpp>
#include <Tests.hpp>

#include <algorithm>
#include <functional>
#include <random>

namespace Ra {
namespace Testing {

void run() {
    static constexpr int nbSamples = 1000;
    static constexpr int randomMin = 1;
    static constexpr int randomMax = 1000;

    using Container = std::vector<int>;

    // Init random number stuff
    std::random_device rnd_device;
    std::mt19937 mersenne_engine( rnd_device() );
    std::uniform_int_distribution<int> dist( randomMin, randomMax );

    // Generate array with random numbers
    auto gen = std::bind( dist, mersenne_engine );
    Container container( nbSamples );
    std::generate( std::begin( container ), std::end( container ), gen );
    std::shuffle( std::begin( container ), std::end( container ), mersenne_engine );

    // check that Ra::Core::reversed produces same results than std::rbegin()
    Container reverted1;
    reverted1.reserve( nbSamples );
    Container reverted2;
    reverted2.reserve( nbSamples );

    for ( auto v : Ra::Core::reversed( container ) )
        reverted1.push_back( v );
    for ( auto vIt = container.rbegin(); vIt != container.rend(); vIt++ )
        reverted2.push_back( *vIt );

    RA_VERIFY( reverted1 == reverted2, "Comparison with std::reverse_iterator" );
    RA_VERIFY( container != reverted1, "Reversed array should be different from original" );

    // check that reverse iterator produce same result than std::algorithm
    Container reverted3( nbSamples );
    std::reverse_copy( container.begin(), container.end(), reverted3.begin() );

    RA_VERIFY( reverted1 == reverted3, "Reversed array equal std::reverse(original)" );

    // check constant access
    const Container& ccontainer = container;
    Container reverted4;
    reverted4.reserve( nbSamples );
    for ( auto v : Ra::Core::reversed( ccontainer ) )
        reverted4.push_back( v );

    RA_VERIFY( reverted1 == reverted4, "Const qualifier should not change the loop behavior" );

    // check we can also write in the containers
    for ( auto& v : Ra::Core::reversed( reverted2 ) )
        v++;
    for ( auto& v : reverted1 )
        v++;
    RA_VERIFY( reverted1 == reverted2, "Writting is working !" );
    RA_VERIFY( reverted1 != reverted3, "Writting is working !" );
}
} // namespace Testing
} // namespace Ra

int main( int argc, const char** argv ) {
    using namespace Ra;

    if ( !Testing::init_testing( 1, argv ) )
    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
    for ( int i = 0; i < Testing::g_repeat; ++i )
    {
        CALL_SUBTEST( ( Testing::run() ) );
    }

    return EXIT_SUCCESS;
}
