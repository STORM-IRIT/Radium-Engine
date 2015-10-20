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
    u.resize( bit.size(), 1 );
    u.setZero();
    for( uint i = 0; i < bit.size(); ++i ) {
        if( bit[i]) {
            u.insert( i, 0 ) = default_value;
        }
    }
}



Delta delta( const Source& source,
             const uint    size,
             const Scalar& default_value ) {
    Delta d( size, 1 );
    for( auto s : source ) {
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
    for( auto s : source ) {
        u.insert( s, 0 )  = default_value;
    }
}



}
}
}
