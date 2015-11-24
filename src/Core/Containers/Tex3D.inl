#include "Tex3D.hpp"

namespace Ra
{
    namespace Core
    {
        template<typename T>
        Tex3D<T>::Tex3D( const Vector3i& resolution,
                         const Vector3& start,
                         const Vector3& end )
            : Grid3<T> ( resolution ), m_aabb( start, end )
        {
            const Vector3 quotient = ( resolution - Vector3i::Ones() ).cast<Scalar>();
            m_cellSize = m_aabb.sizes().cwiseQuotient( quotient );
        }

        template<typename T>
        Tex3D<T>::Tex3D( const Vector3i& resolution, const Aabb& aabb )
            : Grid3<T> ( resolution ), m_aabb( aabb )
        {
            const Vector3 quotient = ( resolution - Vector3i( 1, 1, 1 ) ).cast<Scalar>();
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
            /*
            // TODO : this could be optimized (e.g. by storing 1/cellSize ?)
            Vector3 clamped = Vector::clamp( v, m_aabb.min(), m_aabb.max() );

            Vector3 scaled = ( clamped - m_aabb.min() ).cwiseQuotient( m_cellSize );
            Vector3 gridBaseF = Vector::floor( scaled );
            Vector3 diff = scaled - gridBaseF;

            Vector3i nCells = this->size() - Vector3i::Ones();
            Vector3i gridMin = gridBaseF.cast<int>();
            Vector3i gridMax = ( gridMin + Vector3i::Ones() ).cwiseMin( nCells );

            Eigen::AlignedBox3i cell( gridMin, gridMax );

            const T& v000 = (*this)( cell.corner( Eigen::AlignedBox3i::BottomLeftFloor ) );
            const T& v100 = (*this)( cell.corner( Eigen::AlignedBox3i::BottomRightFloor ) );
            const T& v010 = (*this)( cell.corner( Eigen::AlignedBox3i::TopLeftFloor ) );
            const T& v110 = (*this)( cell.corner( Eigen::AlignedBox3i::TopRightFloor ) );
            const T& v001 = (*this)( cell.corner( Eigen::AlignedBox3i::BottomLeftCeil ) );
            const T& v101 = (*this)( cell.corner( Eigen::AlignedBox3i::BottomRightCeil ) );
            const T& v011 = (*this)( cell.corner( Eigen::AlignedBox3i::TopLeftCeil ) );
            const T& v111 = (*this)( cell.corner( Eigen::AlignedBox3i::TopRightCeil ) );

            const T c00 = v000 * ( 1.f - diff.x() ) + v100 * diff.x();
            const T c10 = v010 * ( 1.f - diff.x() ) + v110 * diff.x();
            const T c01 = v001 * ( 1.f - diff.x() ) + v101 * diff.x();
            const T c11 = v011 * ( 1.f - diff.x() ) + v111 * diff.x();

            const T c0 = c00 * ( 1.f - diff.y() ) + c10 * diff.y();
            const T c1 = c01 * ( 1.f - diff.y() ) + c11 * diff.y();

            T result = c0 * ( 1.f - diff.z() ) + c1 * diff.z();
            return result;*/


            Vector3 scaled_coords( (v -  m_aabb.min() ).cwiseQuotient( m_cellSize ) );
            Vector3 tmp = Vector::floor( scaled_coords );
            Vector3i nearest = Vector3i( int( tmp[0]), int( tmp[1]), int(tmp[2]));

            Vector3 fact = scaled_coords - Vector3( nearest[0], nearest[1], nearest[2] ) ;

            // TODO: FIXME: Test if in bounds and clamp or assert or repeat value...

            Vector3i size = (*this).size() - Vector3i::Ones(); // TODO check this code on borders of the grid
            Vector3i clamped_nearest;
            for( uint i = 0; i < 3; ++i ) {
                clamped_nearest[i] = ( nearest[i] < 0.0 ) ? 0 : ( nearest[i] > size[i] ) ? size[i] : nearest[i];
            }

            const int i0 = clamped_nearest[0];
            const int j0 = clamped_nearest[1];
            const int k0 = clamped_nearest[2];
            const int i1 = i0 < size[0] ? i0 + 1 : i0;
            const int j1 = j0 < size[1] ? j0 + 1 : j0;
            const int k1 = k0 < size[2] ? k0 + 1 : k0;

            const T v000 = (*this)( i0, j0, k0 );
            const T v001 = (*this)( i0, j0, k1 );
            const T v010 = (*this)( i0, j1, k0 );
            const T v011 = (*this)( i0, j1, k1 );
            const T v100 = (*this)( i1, j0, k0 );
            const T v101 = (*this)( i1, j0, k1 );
            const T v110 = (*this)( i1, j1, k0 );
            const T v111 = (*this)( i1, j1, k1 );


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
