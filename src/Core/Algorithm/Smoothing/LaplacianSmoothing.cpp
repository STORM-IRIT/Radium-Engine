#include <Core/Algorithm/Smoothing/LaplacianSmoothing.hpp>

#include <utility>

namespace Ra {
namespace Core {
namespace Algorithm {



ScalarValue smartClamp( const BitSet& bit, const ScalarValue& value, const bool flag ) {
    ScalarValue s( value );
    s.prune( 0.0 );
    for( ScalarValue::InnerIterator it( s, 0 ); it; ++it ) {
        uint   i = it.row();
        Scalar v = it.value();
        if( !( bit[i] || flag ) ) {
            s.coeffRef( i, 0 ) = 0.0;
        }
    }
    return s;
}



void smartClamp( const BitSet& bit, const ScalarValue& value, ScalarValue& clamped, const bool flag ) {
    ScalarValue s( value );
    s.prune( 0.0 );
    for( ScalarValue::InnerIterator it( s, 0 ); it; ++it ) {
        uint   i = it.row();
        Scalar v = it.value();
        if( !( bit[i] || flag ) ) {
            s.coeffRef( i, 0 ) = 0.0;
        }
    }
    std::swap( s, clamped );
}



VectorArray< Vector3 > laplacianSmoothing( const VectorArray< Vector3 >& v, const Geometry::LaplacianMatrix& L, const ScalarValue& weight, const uint iteration ) {
    VectorArray< Vector3 > p( v );
    auto pM = p.getMap();
    VectorArray< Vector3 > tmp( v );
    auto tmpM = tmp.getMap();
    for( uint i = 0; i < iteration; ++i ) {
        tmpM = pM * L;
        for( uint c = 0; c < v.size(); ++c ) {
            tmpM.col( c ) = ( tmpM.col( c ) * weight.coeff( c, 0 ) ) + ( pM.col( c ) * ( 1.0 - weight.coeff( c, 0 ) ) );
        }
        p = tmp;
    }
    return p;
}





}
}
}
