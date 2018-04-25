#include <Core/Geometry/LaplacianSmoothing.hpp>

#include <utility>

namespace Ra {
namespace Core {
namespace Geometry{

ScalarValue smartClamp( const BitSet& bit, const ScalarValue& value, const bool flag ) {
    ScalarValue s( value );
    Scalar zero = 0.0;
    s.prune( zero );
    for ( ScalarValue::InnerIterator it( s, 0 ); it; ++it )
    {
        uint i = it.row();
        if ( !( bit[i] || flag ) )
        {
            s.coeffRef( i, 0 ) = 0.0;
        }
    }
    return s;
}

void smartClamp( const BitSet& bit, const ScalarValue& value, ScalarValue& clamped,
                 const bool flag ) {
    ScalarValue s( value );
    Scalar zero = 0.0;
    s.prune( zero );
    for ( ScalarValue::InnerIterator it( s, 0 ); it; ++it )
    {
        uint i = it.row();
        if ( !( bit[i] || flag ) )
        {
            s.coeffRef( i, 0 ) = 0.0;
        }
    }
    s.swap( clamped );
}

Container::VectorArray<Math::Vector3> laplacianSmoothing( const Container::VectorArray<Math::Vector3>& v,
                                         const LaplacianMatrix& L,
                                         const ScalarValue& weight, const uint iteration ) {
    Container::VectorArray<Math::Vector3> p( v );
    auto pM = p.getMap();
    Container::VectorArray<Math::Vector3> tmp( v );
    auto tmpM = tmp.getMap();
    for ( uint i = 0; i < iteration; ++i )
    {
        tmpM = pM * L;
        for ( uint c = 0; c < v.size(); ++c )
        {
            tmpM.col( c ) = ( tmpM.col( c ) * weight.coeff( c, 0 ) ) +
                            ( pM.col( c ) * ( 1.0 - weight.coeff( c, 0 ) ) );
        }
        pM.swap( tmpM );
    }
    return tmp;
}

void laplacianSmoothing( const Container::VectorArray<Math::Vector3>& v,
                         const LaplacianMatrix& L, const ScalarValue& weight,
                         const uint iteration, Container::VectorArray<Math::Vector3>& p ) {
    p = v;
    auto pM = p.getMap();
    Container::VectorArray<Math::Vector3> tmp( v );
    auto tmpM = tmp.getMap();
    for ( uint i = 0; i < iteration; ++i )
    {
        tmpM = pM * L;
        for ( uint c = 0; c < v.size(); ++c )
        {
            tmpM.col( c ) = ( tmpM.col( c ) * weight.coeff( c, 0 ) ) +
                            ( pM.col( c ) * ( 1.0 - weight.coeff( c, 0 ) ) );
        }
        pM.swap( tmpM );
    }
    std::swap( p, tmp );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
