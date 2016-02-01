#include "Tex3D.hpp"

namespace Ra
{
    namespace Core
    {
        template<typename T>
        Tex3D<T>::Tex3D( const Vector3ui& resolution,
                         const Vector3& start,
                         const Vector3& end )
            : Grid<T,3> ( resolution ), m_aabb( start, end )
        {
            const Vector3 quotient = ( resolution - Vector3ui::Ones() ).cast<Scalar>();
            m_cellSize = m_aabb.sizes().cwiseQuotient( quotient );
        }

        template<typename T>
        Tex3D<T>::Tex3D(const Vector3ui &resolution, const Aabb& aabb )
            : Grid<T,3> ( resolution ), m_aabb( aabb )
        {
            const Vector3 quotient = ( resolution - Vector3ui::Ones() ).cast<Scalar>();
            m_cellSize = m_aabb.sizes().cwiseQuotient( quotient );
        }

        template<typename T>
        inline const Aabb& Tex3D<T>::getAabb() const
        {
            return m_aabb;
        }

        template<typename T>
        inline T Tex3D<T>::fetch( const Vector3& v ) const
        {
            Vector3 scaled_coords( (v -  m_aabb.min() ).cwiseQuotient( m_cellSize ) );
            Vector3 tmp = Vector::floor( scaled_coords );
            Vector3ui nearest = tmp.cast<uint>();

            Vector3 fact = scaled_coords - Vector3( nearest[0], nearest[1], nearest[2] ) ;

            // TODO: FIXME: Test if in bounds and clamp or assert or repeat value...

            Vector3ui size = this->sizeVector() - Vector3ui::Ones(); // TODO check this code on borders of the grid
            Vector3ui clamped_nearest = Ra::Core::Vector::clamp<Vector3ui>( nearest, Vector3ui::Zero(), size);

            const uint i0 = clamped_nearest[0];
            const uint j0 = clamped_nearest[1];
            const uint k0 = clamped_nearest[2];
            const uint i1 = i0 < size[0] ? i0 + 1 : i0;
            const uint j1 = j0 < size[1] ? j0 + 1 : j0;
            const uint k1 = k0 < size[2] ? k0 + 1 : k0;

            const T v000 = this->at( {i0, j0, k0 } );
            const T v001 = this->at( {i0, j0, k1 } );
            const T v010 = this->at( {i0, j1, k0 } );
            const T v011 = this->at( {i0, j1, k1 } );
            const T v100 = this->at( {i1, j0, k0 } );
            const T v101 = this->at( {i1, j0, k1 } );
            const T v110 = this->at( {i1, j1, k0 } );
            const T v111 = this->at( {i1, j1, k1 } );

            const T c00 = v000 * (1.0 - fact[0]) + v100 * fact[0];
            const T c10 = v010 * (1.0 - fact[0]) + v110 * fact[0];
            const T c01 = v001 * (1.0 - fact[0]) + v101 * fact[0];
            const T c11 = v011 * (1.0 - fact[0]) + v111 * fact[0];

            const T c0 = c00 * ( 1.0 - fact[1] ) + c10 * fact[1];
            const T c1 = c01 * ( 1.0 - fact[1] ) + c11 * fact[1];

            return c0 * (1.0 - fact[2]) + c1 * fact[2];
        }
    }
}
