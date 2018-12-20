#include <Core/Utils/IndexMap.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {
namespace IndexMap {

// Just a standard test structure
struct Foo {
    Foo( int x ) : value( x ) {}
    int value;
};

} // namespace IndexMap

void runMapTest() {
    using Ra::Core::Utils::Index;
    using Ra::Core::Utils::IndexMap;
    using Ra::Testing::IndexMap::Foo;

    IndexMap<Foo> map1;

    // Sanity checks
    RA_VERIFY( map1.empty(), "New map should be empty" );
    RA_VERIFY( map1.size() == 0, "New map should be empty (size)" );
    RA_VERIFY( !map1.full(), "New map should not be full" );

    // Inserting
    Index i1 = map1.insert( Foo( 12 ) );
    RA_VERIFY( i1.isValid(), "Insert" );

    // We expect to have one element now.
    RA_VERIFY( !map1.empty(), "Map should be non-empty" );
    RA_VERIFY( map1.size() == 1, "Size is wrong" );

    // Test value read and write
    RA_VERIFY( map1.at( i1 ).value == 12, "map read (at)" );
    RA_VERIFY( map1[i1].value == 12, "map read (operator[])" );

    map1.access( i1 ).value = 24;
    RA_VERIFY( map1.at( i1 ).value == 24, "map write (access)" );

    map1[i1].value = 32;
    RA_VERIFY( map1.at( i1 ).value == 32, "map write (operator[])" );

    // Inserting a second element
    Index i2 = map1.insert( Foo( 42 ) );
    RA_VERIFY( map1.at( i2 ).value == 42, "map read " );
    RA_VERIFY( map1.size() == 2, "Size is wrong" );

    // Test range-based for loop (relies on iterators)
    uint counter = 0;
    for ( const Foo& f : map1 )
    {
        RA_VERIFY( f.value == 32 || f.value == 42, "Invalid value (const loop)" );
        ++counter;
    }
    RA_VERIFY( counter == 2, "Range-for invalid count (const loop)" );

    // Test non-const loop
    counter = 0;
    for ( Foo& f : map1 )
    {
        f.value = 2 * f.value;
        RA_VERIFY( f.value == 2 * 32 || f.value == 2 * 42, "Invalid value (non-const loop)" );
        ++counter;
    }
    RA_VERIFY( counter == 2, "Range-for invalid count( non-const loop)" );

    // Test index iterators
    counter = 0;
    for ( auto it = map1.cbegin_index(); it != map1.cend_index(); ++it )
    {
        RA_VERIFY( *it == i1 || *it == i2, "Invalid index (index loop)" );
        ++counter;
    }

    // Test 'contains'
    RA_VERIFY( map1.contains( i1 ), "map contains" );
    RA_VERIFY( map1.contains( i2 ), "map contains" );
    RA_VERIFY( !map1.contains( Index( 12000 ) ), "Map contains" );

    // Remove an item once.
    bool result = map1.remove( i1 );
    RA_VERIFY( result, "Map remove (auto index)" );
    RA_VERIFY( map1.size() == 1, "Map remove (size)" );

    // Removing it twice should not work.
    result = map1.remove( i1 );
    RA_VERIFY( !result, "Map remove (twice)" );

    // Remove the other item
    result = map1.remove( Index( 1000 ) );
    RA_VERIFY( !result, "Map remove (non-exitent index)" );

    result = map1.remove( i2 );
    RA_VERIFY( result, "Map remove" );

    RA_VERIFY( map1.size() == 0, "Map should be empty (size)" );
    RA_VERIFY( map1.empty(), "Map should be empty" );
}

void runMapNonCopyableTest() {
    using Ra::Core::Utils::Index;
    using Ra::Core::Utils::IndexMap;
    // Now try to insert non-copyable elements

    IndexMap<NonCopy> map2;
    Index i1 = map2.emplace( std::move( 12 ) );
    RA_VERIFY( i1.isValid(), "map insert (inplace)" );

    Index i2 = map2.emplace( std::move( 42 ) );
    RA_VERIFY( i2.isValid(), "map insert (inplace)" );

    RA_VERIFY( map2[i1].value == 12, "map access(inplace)" );
    RA_VERIFY( map2[i2].value == 42, "map access(inplace)" );

    // Test the clear function
    map2.clear();
    RA_VERIFY( map2.size() == 0, "map clear (size)" );
    RA_VERIFY( map2.empty(), "map clear (empty)" );
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
        CALL_SUBTEST( ( Testing::runMapTest() ) );
        CALL_SUBTEST( ( Testing::runMapNonCopyableTest() ) );
    }

    return EXIT_SUCCESS;
}
