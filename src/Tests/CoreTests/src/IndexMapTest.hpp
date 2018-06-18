#ifndef RADIUM_INDEXMAP_TEST_HPP_
#define RADIUM_INDEXMAP_TEST_HPP_

#include <Core/Index/IndexMap.hpp>
#include <Tests/Tests.hpp>

namespace RaTests {
using Ra::Core::Index;
using Ra::Core::IndexMap;

// Just a standard test structure
struct Foo {
    Foo( int x ) : value( x ) {}
    int value;
};

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

class IndexMapTest : public Test {
    void run() override {
        {
            IndexMap<Foo> map1;

            // Sanity checks
            RA_UNIT_TEST( map1.empty(), "New map should be empty" );
            RA_UNIT_TEST( map1.size() == 0, "New map should be empty (size)" );
            RA_UNIT_TEST( !map1.full(), "New map should not be full" );

            // Inserting
            Index i1 = map1.insert( Foo( 12 ) );
            RA_UNIT_TEST( i1.isValid(), "Insert" );

            // We expect to have one element now.
            RA_UNIT_TEST( !map1.empty(), "Map should be non-empty" );
            RA_UNIT_TEST( map1.size() == 1, "Size is wrong" );

            // Test value read and write
            RA_UNIT_TEST( map1.at( i1 ).value == 12, "map read (at)" );
            RA_UNIT_TEST( map1[i1].value == 12, "map read (operator[])" );

            map1.access( i1 ).value = 24;
            RA_UNIT_TEST( map1.at( i1 ).value == 24, "map write (access)" );

            map1[i1].value = 32;
            RA_UNIT_TEST( map1.at( i1 ).value == 32, "map write (operator[])" );

            // Inserting a second element
            Index i2 = map1.insert( Foo( 42 ) );
            RA_UNIT_TEST( map1.at( i2 ).value == 42, "map read " );
            RA_UNIT_TEST( map1.size() == 2, "Size is wrong" );

            // Test range-based for loop (relies on iterators)
            uint counter = 0;
            for ( const Foo& f : map1 )
            {
                RA_UNIT_TEST( f.value == 32 || f.value == 42, "Invalid value (const loop)" );
                ++counter;
            }
            RA_UNIT_TEST( counter == 2, "Range-for invalid count (const loop)" );

            // Test non-const loop
            counter = 0;
            for ( Foo& f : map1 )
            {
                f.value = 2 * f.value;
                RA_UNIT_TEST( f.value == 2 * 32 || f.value == 2 * 42,
                              "Invalid value (non-const loop)" );
                ++counter;
            }
            RA_UNIT_TEST( counter == 2, "Range-for invalid count( non-const loop)" );

            // Test index iterators
            counter = 0;
            for ( auto it = map1.cbegin_index(); it != map1.cend_index(); ++it )
            {
                RA_UNIT_TEST( *it == i1 || *it == i2, "Invalid index (index loop)" );
                ++counter;
            }

            // Test 'contains'
            RA_UNIT_TEST( map1.contains( i1 ), "map contains" );
            RA_UNIT_TEST( map1.contains( i2 ), "map contains" );
            RA_UNIT_TEST( !map1.contains( Index( 12000 ) ), "Map contains" );

            // Remove an item once.
            bool result = map1.remove( i1 );
            RA_UNIT_TEST( result, "Map remove (auto index)" );
            RA_UNIT_TEST( map1.size() == 1, "Map remove (size)" );

            // Removing it twice should not work.
            result = map1.remove( i1 );
            RA_UNIT_TEST( !result, "Map remove (twice)" );

            // Remove the other item
            result = map1.remove( Index( 1000 ) );
            RA_UNIT_TEST( !result, "Map remove (non-exitent index)" );

            result = map1.remove( i2 );
            RA_UNIT_TEST( result, "Map remove" );

            RA_UNIT_TEST( map1.size() == 0, "Map should be empty (size)" );
            RA_UNIT_TEST( map1.empty(), "Map should be empty" );
        }

        // Now try to insert non-copyable elements
        {
            IndexMap<NonCopy> map2;
            Index i1 = map2.emplace( std::move( 12 ) );
            RA_UNIT_TEST( i1.isValid(), "map insert (inplace)" );

            Index i2 = map2.emplace( std::move( 42 ) );
            RA_UNIT_TEST( i2.isValid(), "map insert (inplace)" );

            RA_UNIT_TEST( map2[i1].value == 12, "map access(inplace)" );
            RA_UNIT_TEST( map2[i2].value == 42, "map access(inplace)" );

            // Test the clear function
            map2.clear();
            RA_UNIT_TEST( map2.size() == 0, "map clear (size)" );
            RA_UNIT_TEST( map2.empty(), "map clear (empty)" );
        }
    }
};
RA_TEST_CLASS( IndexMapTest );

} // namespace RaTests

#endif // RADIUM_INDEXMAP_TEST_HPP_
