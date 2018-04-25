#ifndef RADIUMENGINE_ASSIMP_WRAPPER_HPP
#define RADIUMENGINE_ASSIMP_WRAPPER_HPP

#include <assimp/color4.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector3.h>

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace IO {

inline Core::Math::Vector3 assimpToCore( const aiVector3D& v ) {
    return Core::Math::Vector3( v.x, v.y, v.z );
}

inline aiVector3D coreToAssimp( const Core::Math::Vector3& v ) {
    return aiVector3D( v.x(), v.y(), v.z() );
}

inline Core::Math::Quaternion assimpToCore( const aiQuaternion& q ) {
    return Core::Math::Quaternion( q.w, q.x, q.y, q.z );
}

inline Core::Math::Transform assimpToCore( const aiMatrix4x4& M ) {
    Core::Math::Matrix4 m( Core::Math::Matrix4::Identity() );
    for ( uint i = 0; i < 4; ++i )
    {
        for ( uint j = 0; j < 4; ++j )
        {
            m( i, j ) = M[i][j];
        }
    }
    return Core::Math::Transform( m );
}

inline Core::Math::Transform assimpToCore( const aiVector3D& T, const aiQuaternion& R,
                                     const aiVector3D& S ) {
    Core::Math::Vector3 t = assimpToCore( T );
    Core::Math::Quaternion r = assimpToCore( R );
    Core::Math::Vector3 s = assimpToCore( S );
    Core::Math::Transform M;
    M.fromPositionOrientationScale( t, r, s );
    return M;
}

inline Core::Math::Color assimpToCore( const aiColor4D& c ) {
    return Core::Math::Color( c.r, c.g, c.b, c.a );
}

inline std::string assimpToCore( const aiString& string ) {
    std::string result( string.C_Str() );
    std::transform( result.begin(), result.end(), result.begin(), []( char in ) {
        if ( in == '\\' )
            return '/';
        return in;
    } );
    return result.empty() ? "default" : result;
}

inline Core::Math::VectorNi assimpToCore( uint* index, const uint size ) {
    Core::Math::VectorNi v( size );
    for ( uint i = 0; i < size; ++i )
    {
        v[i] = index[i];
    }

    return v;
}

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_WRAPPER_HPP
