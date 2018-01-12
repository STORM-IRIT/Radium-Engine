#include <Core/Algorithm/Delta/Delta.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {



Delta delta( const BitSet& bit,
             const Scalar& default_value ) {
    Delta u( bit.size(), 1 );
    for( uint i = 0; i < bit.size(); ++i ) {
        if( bit[i]) {
            u.insert( i, 0 ) = default_value;
        }
    }
    return u;
}



void delta( const BitSet& bit,
            Delta&        u,
            const Scalar& default_value ) {
    Delta d( bit.size(), 1 );
#pragma omp parallel for
    for( int i = 0; i < int(bit.size()); ++i ) {
        if( bit[i]) {
#pragma omp critical
{
            d.insert( i, 0 ) = default_value;
}
        }
    }
#pragma omp barrier
    std::swap( u, d );
}



Delta delta( const Source& source,
             const uint    size,
             const Scalar& default_value ) {
    Delta d( size, 1 );
    for( const auto& s: source ) {
        d.insert( s, 0 )  = default_value;
    }
    return d;
}



void delta( const Source& source,
            const uint    size,
            Delta&        u,
            const Scalar& default_value ) {
    u.resize( size, 1 );
    u.setZero();
    u.reserve( source.size() );
    for( const auto& s: source ) {
        u.insert( s, 0 )  = default_value;
    }
}



void bitset( const Delta& u, BitSet& bit ) {
    const uint n = u.rows();
    bit.resize( n, false );
    for( Delta::InnerIterator it( u, 0 ); it; ++it ) {
        uint i = it.row();
        bit[i] = true;
    }
}



}
}
}
