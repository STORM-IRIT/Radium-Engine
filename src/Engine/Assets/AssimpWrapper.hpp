#ifndef RADIUMENGINE_ASSIMP_WRAPPER_HPP
#define RADIUMENGINE_ASSIMP_WRAPPER_HPP

#include <assimp/vector3.h>
#include <assimp/quaternion.h>
#include <assimp/matrix4x4.h>
#include <assimp/color4.h>
#include <assimp/types.h>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Asset {

inline Core::Vector3 assimpToCore( const aiVector3t< Scalar >& v ) {
    return Core::Vector3( v.x, v.y, v.z );
}

inline Core::Quaternion assimpToCore( const aiQuaterniont< Scalar >& q ) {
    return Core::Quaternion( q.w, q.x, q.y, q.z );
}

inline Core::Transform assimpToCore( const aiMatrix4x4t< Scalar >& M ) {
    Core::Matrix4 m( Core::Matrix4::Identity() );
    for( uint i = 0; i < 4; ++i )
    {
        for( uint j = 0; j < 4; ++j )
        {
            m( i, j ) = M[i][j];
        }
    }
    return Core::Transform( m );
}

inline Core::Transform assimpToCore( const aiVector3t< Scalar >&    T,
                                     const aiQuaterniont< Scalar >& R,
                                     const aiVector3t< Scalar >&    S ) {
    Core::Vector3    t = assimpToCore( T );
    Core::Quaternion r = assimpToCore( R );
    Core::Vector3    s = assimpToCore( S );
    Core::Transform M;
    M.fromPositionOrientationScale( t, r, s );
    return M;
}

inline Core::Color assimpToCore( const aiColor4t< Scalar >& c ) {
//inline Core::Color assimpToCore( const aiColor4D& c ) {
    return Core::Color( c.r, c.g, c.b, c.a );
}

inline std::string assimpToCore( const aiString& string ) {
    return std::string( string.C_Str() );
}

inline Core::VectorNi assimpToCore( uint* index, const uint size )
{
    Core::VectorNi v( size );
    for( uint i = 0; i < size; ++i ) {
        v[i] = index[i];
    }

    return v;
}

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_WRAPPER_HPP
