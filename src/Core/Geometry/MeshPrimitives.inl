#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Math/LinearAlgebra.hpp> // Math::pi

namespace Ra {
namespace Core {
namespace Geometry {

template <uint U, uint V>
TriangleMesh makeParametricSphere( Scalar radius, const Utils::optional<Utils::Color>& color ) {
    constexpr uint slices = U;
    constexpr uint stacks = V;

    TriangleMesh result;
    result.vertices().reserve( 2 + slices * ( stacks - 1 ) );
    result.normals().reserve( 2 + slices * ( stacks - 1 ) );
    result.m_triangles.reserve( 2 * slices * ( stacks - 1 ) );

    for ( uint u = 0; u < slices; ++u )
    {
        const Scalar theta = Scalar( 2 * u ) * Core::Math::Pi / Scalar( slices );
        for ( uint v = 1; v < stacks; ++v )
        {
            // Regular vertices on the sphere.
            const Scalar phi = Scalar( v ) * Core::Math::Pi / Scalar( stacks );
            result.vertices().push_back( Vector3( radius * std::cos( theta ) * std::sin( phi ),
                                                  radius * std::sin( theta ) * std::sin( phi ),
                                                  radius * std::cos( phi ) ) );
            result.normals().push_back( result.vertices().back().normalized() );

            // Regular triangles
            if ( v > 1 )
            {
                const uint nextSlice = ( ( u + 1 ) % slices ) * ( stacks - 1 );
                const uint baseSlice = u * ( stacks - 1 );
                result.m_triangles.push_back(
                    Vector3ui( baseSlice + v - 2, baseSlice + v - 1, nextSlice + v - 1 ) );
                result.m_triangles.push_back(
                    Vector3ui( baseSlice + v - 2, nextSlice + v - 1, nextSlice + v - 2 ) );
            }
        }
    }

    // Add the pole vertices.
    uint northPoleIdx = result.vertices().size();
    result.vertices().push_back( Vector3( 0, 0, radius ) );
    result.normals().push_back( Vector3( 0, 0, 1 ) );
    uint southPoleIdx = result.vertices().size();
    result.vertices().push_back( Vector3( 0, 0, -radius ) );
    result.normals().push_back( Vector3( 0, 0, -1 ) );

    // Add the polar caps triangles.
    for ( uint u = 0; u < slices; ++u )
    {
        const uint nextSlice = ( ( u + 1 ) % slices ) * ( stacks - 1 );
        const uint baseSlice = u * ( stacks - 1 );
        result.m_triangles.push_back( Vector3ui( northPoleIdx, baseSlice, nextSlice ) );
        result.m_triangles.push_back(
            Vector3ui( southPoleIdx, nextSlice + stacks - 2, baseSlice + stacks - 2 ) );
    }

    if ( bool( color ) )
        result.colorize( *color );
    result.checkConsistency();

    return result;
}

template <uint U, uint V>
TriangleMesh makeParametricTorus( Scalar majorRadius, Scalar minorRadius,
                                  const Utils::optional<Utils::Color>& color ) {
    TriangleMesh result;
    result.vertices().reserve( U * V );
    result.normals().reserve( V * V );
    result.m_triangles.reserve( 2 * U * V );

    for ( uint iu = 0; iu < U; ++iu )
    {
        Scalar u = Scalar( iu ) * Core::Math::PiMul2 / Scalar( U );
        Core::Vector3 circleCenter( majorRadius * std::cos( u ), majorRadius * std::sin( u ), 0.f );

        for ( uint iv = 0; iv < V; ++iv )
        {
            Scalar v = Scalar( iv ) * Core::Math::PiMul2 / Scalar( V );

            Core::Vector3 vertex( ( majorRadius + minorRadius * std::cos( v ) ) * std::cos( u ),
                                  ( majorRadius + minorRadius * std::cos( v ) ) * std::sin( u ),
                                  minorRadius * std::sin( v ) );

            result.vertices().push_back( vertex );
            result.normals().push_back( ( vertex - circleCenter ).normalized() );

            result.m_triangles.push_back( Vector3ui( iu * V + iv, ( ( iu + 1 ) % U ) * V + iv,
                                                     iu * V + ( ( iv + 1 ) % V ) ) );
            result.m_triangles.push_back( Vector3ui( ( ( iu + 1 ) % U ) * V + iv,
                                                     ( ( iu + 1 ) % U ) * V + ( ( iv + 1 ) % V ),
                                                     iu * V + ( ( iv + 1 ) % V ) ) );
        }
    }

    if ( bool( color ) )
        result.colorize( *color );
    result.checkConsistency();

    return result;
}
} // namespace Geometry
} // namespace Core
} // namespace Ra
