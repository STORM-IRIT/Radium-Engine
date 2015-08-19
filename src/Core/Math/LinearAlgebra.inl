#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        //
        // Vector functions.
        //

        template<typename Vector_>
        inline Vector_ Vector::floor( const Vector_& v )
        {
            typedef typename Vector_::Scalar Scalar_;
            Vector_ result( v );
            result.unaryExpr( std::function<Scalar_( Scalar_ )> ( static_cast<Scalar_( & )( Scalar_ )> ( std::floor ) ) );
            return result;
        }

        template<typename Vector_>
        inline Vector_ Vector::ceil( const Vector_& v )
        {
            typedef typename Vector_::Scalar Scalar_;
            Vector_ result( v );
            result.unaryExpr( std::function<Scalar_( Scalar_ )> ( static_cast<Scalar_( & )( Scalar_ )> ( std::ceil ) ) );
            return result;
        }

        template<typename Vector_>
        inline Vector_ Vector::clamp( const Vector_& v, const Vector_& min, const Vector_& max )
        {
            return v.cwiseMin( max ).cwiseMax( min );
        }

        template<typename Vector_>
        inline Vector_ Vector::clamp( const Vector_& v, const Scalar& min, const Scalar& max )
        {
            return v.cwiseMin( max ).cwiseMax( min );
        }

        template<typename Vector_>
        inline bool Vector::checkRange( const Vector_& v, const Scalar& min, const Scalar& max )
        {
            return Vector::clamp( v, min, max ) == v;
        }

        //
        // Quaternion functions.
        //

        Quaternion operator+ ( const Quaternion& q1, const Quaternion& q2 )
        {
            return Quaternion( q1.coeffs() + q2.coeffs() );
        }

        Quaternion operator* ( const Scalar& k, const Quaternion& q )
        {
            return Quaternion( k * q.coeffs() );
        }

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

        inline void Obb::addPoint( const Vector3& p )
        {
            // TODO: take the transform into account, but then apply the changes in capsule_implicit.cpp
            m_aabb.extend( p );
        }

        inline void getOrthogonalVectors( const Vector3& fx, Vector3& fy, Vector3& fz )
        {
            //for numerical stability, and seen that z will always be present, take the greatest component between x and y.
            if ( fabsf( fx( 0 ) ) > fabsf( fx( 1 ) ) )
            {
                float inv_len = 1.f / sqrtf( fx( 0 ) * fx( 0 ) + fx( 2 ) * fx( 2 ) );
                Vector3 tmp( -fx( 2 ) * inv_len, 0.f, fx( 0 ) * inv_len );
                fy = tmp;
            }
            else
            {
                float inv_len = 1.f / sqrtf( fx( 1 ) * fx( 1 ) + fx( 2 ) * fx( 2 ) );
                Vector3 tmp( 0.f, fx( 2 ) * inv_len, -fx( 1 ) * inv_len );
                fy = tmp;
            }
            fz = fx.cross( fy );
        }

    }
}
