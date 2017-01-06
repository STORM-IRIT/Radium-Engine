

namespace Ra
{
namespace Core
{

    inline void print( const MatrixN& matrix ) {
        // Taken straight from :
        // http://eigen.tuxfamily.org/dox/structEigen_1_1IOFormat.html

        std::cout << "Matrix rows : " << matrix.rows() << std::endl;
        std::cout << "Matrix cols : " << matrix.cols() << std::endl;
        Eigen::IOFormat cleanFormat( 4, 0, ", ", "\n", "[", "]" );
        std::cout << matrix.format( cleanFormat ) << std::endl;
    }

    //
    // Vector functions.
    //

    template<typename Vector_>
    inline Vector_ Vector::floor( const Vector_& v )
    {
        typedef typename Vector_::Scalar Scalar_;
        return v.unaryExpr( std::function<Scalar_( Scalar_ )> ( static_cast<Scalar_( & )( Scalar_ )> ( std::floor ) ) );
    }

    template<typename Vector_>
    inline Vector_ Vector::ceil( const Vector_& v )
    {
        typedef typename Vector_::Scalar Scalar_;
        return v.unaryExpr( std::function<Scalar_( Scalar_ )> ( static_cast<Scalar_( & )( Scalar_ )> ( std::ceil ) ) );
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

    template <typename Vector_>
    inline Scalar Vector::angle( const Vector_& v1, const Vector_& v2 )
    {
        return std::atan2( v1.cross(v2).norm(), v1.dot(v2));
    }

    template<typename Vector_>
    Scalar Vector::cotan(const Vector_& v1, const Vector_& v2)
    {
        return  v1.dot(v2) / v1.cross(v2).norm();
    }

    template<typename Vector_>
    Scalar Vector::cos(const Vector_& v1, const Vector_& v2)
    {
        return  (v1.dot(v2)) / std::sqrt( v1.normSquared() * v2.normSquared());
    }

    //
    // Quaternion functions.
    //

    inline Quaternion QuaternionUtils::add ( const Quaternion& q1, const Quaternion& q2 )
    {
        return Quaternion( q1.coeffs() + q2.coeffs() );
    }

    inline Quaternion QuaternionUtils::addQlerp ( const Quaternion& q1, const Quaternion& q2 )
    {
        const Scalar sign = Ra::Core::Math::signNZ(q1.dot(q2));
        return Quaternion( q1.coeffs() + (sign * q2.coeffs()) );
    }

    inline Quaternion QuaternionUtils::scale( const Quaternion& q, Scalar k )
    {
        return Quaternion( k * q.coeffs() );
    }

    inline void Vector::getOrthogonalVectors( const Vector3& fx, Vector3& fy, Vector3& fz )
    {
        //for numerical stability, and seen that z will always be present, take the greatest component between x and y.
        if ( std::abs( fx( 0 ) ) > std::abs( fx( 1 ) ) )
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

    inline Vector3 Vector::projectOnPlane(const Vector3& planePos, const Vector3 planeNormal, const Vector3& point)
    {
        return point + planeNormal * (planePos - point).dot(planeNormal);
    }


    // Reference : Paolo Baerlocher, Inverse Kinematics techniques for interactive posture control
    // of articulated figures (PhD Thesis), p.138. EPFL, 2001.
    // http://www0.cs.ucl.ac.uk/research/equator/papers/Documents2002/Paolo%20Baerlocher_Thesis_2001.pdf
    inline void QuaternionUtils::getSwingTwist(const Quaternion &in, Quaternion &swingOut, Quaternion &twistOut)
    {
        // singular case.
        if ( UNLIKELY(in.z() == 0 && in.w() == 0 ) )
        {
            twistOut = in;
            swingOut.setIdentity();
        }
        else
        {
            const Scalar gamma = std::atan2( in.z(), in.w() );
            // beta = atan2 ( sqrt ( in.x^2 + in.y^2), sqrt ( in.z^2 + in.w^2))
            const Scalar beta = std::atan2( in.coeffs().head<2>().norm(), in.coeffs().tail<2>().norm());

            // k = 2 / sinc(beta) = 2 / ( sin(beta)/ beta))
            const Scalar k = 2.f * beta / std::sin(beta);

            // The parentheses here are important. because otherwise k * rotation2D would
            // be evaluated first (which would yield the rotation of angle k * gamma).
            const Vector2 sxy = k * (Eigen::Rotation2D<Scalar>( gamma ) * in.coeffs().head<2>());

            AngleAxis twist( 2 * gamma, Vector3::UnitZ());
            twistOut = twist;

            Vector3 swingAxis( Vector3::Zero());
            swingAxis.head<2>() = sxy.normalized();

            AngleAxis swing ( sxy.norm(), swingAxis);
            swingOut = swing;
        }
    }

    namespace MatrixUtils
    {
        // http://stackoverflow.com/a/13786235/4717805
        inline Matrix4 lookAt(const Vector3& position, const Vector3& target, const Vector3& up)
        {
            Matrix3 R;
            R.col(2) = (position - target).normalized();
            R.col(0) = up.cross(R.col(2)).normalized();
            R.col(1) = R.col(2).cross(R.col(0));

            Matrix4 result = Matrix4::Zero();
            result.topLeftCorner<3, 3>() = R.transpose();
            result.topRightCorner<3, 1>() = -R.transpose() * position;
            result(3, 3) = 1.0;

            return result;
        }

        inline Matrix4 perspective(Scalar fovy, Scalar aspect, Scalar znear, Scalar zfar)
        {
            Scalar theta = fovy * 0.5;
            Scalar range = zfar - znear;
            Scalar invtan = 1.0 / std::tan(theta);

            Matrix4 result = Matrix4::Zero();
            result(0, 0) = invtan / aspect;
            result(1, 1) = invtan;
            result(2, 2) = -(znear + zfar) / range;
            result(3, 2) = -1.0;
            result(2, 3) = -2 * znear * zfar / range;
            result(3, 3) = 0.0;

            return result;
        }

        inline Matrix4 orthographic(Scalar l, Scalar r, Scalar b, Scalar t, Scalar n, Scalar f)
        {
            Matrix4 result = Matrix4::Zero();

            result(0, 0) =  2.0 / (r - l);
            result(1, 1) =  2.0 / (t - b);
            result(2, 2) = -2.0 / (f - n);
            result(3, 3) =  1.0;

            result(0, 3) = -(r + l) / (r - l);
            result(1, 3) = -(t + b) / (t - b);
            result(2, 3) = -(f + b) / (f - n);

            return result;
        }
    }

} // namespace Core
} // namespace Ra

// Insert quaternions operator in Eigen namespace to allow functions outside of
// Ra::Core to use them without explicit calls. This is syntactic sugar,
// but allows to write expressions such as q = ( a*q1 + b*q2 ).normalized();
// anywhere in the code.
// See

namespace Eigen
{
    inline Quaternion<Scalar> operator*(Scalar k, const Quaternion <Scalar>& q)
    {
        return Ra::Core::QuaternionUtils::scale(q, k);
    }

    inline Quaternion <Scalar> operator*( const Quaternion <Scalar>& q, Scalar k)
    {
        return Ra::Core::QuaternionUtils::scale(q, k);
    }

    inline Quaternion <Scalar> operator+(const Quaternion <Scalar>& q1, const Quaternion<Scalar>& q2)
    {
        return Ra::Core::QuaternionUtils::add(q1,q2);
    }

    inline Quaternion<Scalar> operator/ (const Quaternion<Scalar>& q, Scalar k)
    {
        return Ra::Core::QuaternionUtils::scale(q, Scalar(1)/k);
    }
}
