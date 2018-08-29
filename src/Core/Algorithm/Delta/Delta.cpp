#include <Core/Algorithm/Delta/Delta.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

Delta delta( const BitSet& bit, const Scalar& default_value ) {
    using T = Eigen::Triplet<Scalar>;
    std::vector<T> t;
    for ( uint i = 0; i < bit.size(); ++i )
    {
        if ( bit[i] )
        {
            t.push_back( T( i, 0, default_value ) );
        }
    }
    Delta u( bit.size(), 1 );
    u.setFromTriplets( t.begin(), t.end() );
    return u;
}

void delta( const BitSet& bit, Delta& u, const Scalar& default_value ) {
    using T = Eigen::Triplet<Scalar>;
    std::vector<T> t;
    for ( int i = 0; i < int( bit.size() ); ++i )
    {
        if ( bit[i] )
        {
            t.push_back( T( i, 0, default_value ) );
        }
    }
    Delta d( bit.size(), 1 );
    d.setFromTriplets( t.begin(), t.end() );
    std::swap( u, d );
}

Delta delta( const Source& source, const uint size, const Scalar& default_value ) {
    using T = Eigen::Triplet<Scalar>;
    std::vector<T> t;
    t.reserve( source.size() );
    for ( const auto& s : source )
    {
        t.push_back( T( s, 0, default_value ) );
    }
    Delta d( size, 1 );
    d.setFromTriplets( t.begin(), t.end() );
    return d;
}

void delta( const Source& source, const uint size, Delta& u, const Scalar& default_value ) {
    using T = Eigen::Triplet<Scalar>;
    std::vector<T> t;
    t.reserve( source.size() );
    for ( const auto& s : source )
    {
        t.push_back( T( s, 0, default_value ) );
    }
    Delta d( source.size(), 1 );
    d.setFromTriplets( t.begin(), t.end() );
    std::swap( u, d );
}

void bitset( const Delta& u, BitSet& bit ) {
    const uint n = u.rows();
    bit.resize( n, false );
    for ( Delta::InnerIterator it( u, 0 ); it; ++it )
    {
        uint i = it.row();
        bit[i] = true;
    }
}

} // namespace Algorithm
} // namespace Core
} // namespace Ra
