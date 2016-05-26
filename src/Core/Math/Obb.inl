#include "Obb.hpp"

namespace Ra
{
    namespace Core
    {
        //
        // Bounding boxes functions.
        //

        inline Aabb Obb::toAabb() const
        {
            Aabb tmp;
            for ( int i = 0; i < 8; ++i )
            {
                tmp.extend( m_transform * m_aabb.corner( static_cast<Aabb::CornerType>( i ) ) );
            }
            return tmp;
        }

        inline Vector3 Obb::corner(int i) const
        {
            return m_aabb.corner(static_cast<Aabb::CornerType>(i));
        }

        inline Vector3 Obb::worldCorner( int i ) const
        {
            return m_transform * m_aabb.corner( static_cast<Aabb::CornerType>( i ) );
        }

        inline void Obb::addPoint( const Vector3& p )
        {
            m_aabb.extend( p );
        }
    }
}
