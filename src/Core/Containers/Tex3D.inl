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
            return result;
        }
    }
}
