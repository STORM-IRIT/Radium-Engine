#ifndef RADIUMENGINE_DUALQUATERNION_HPP
#define RADIUMENGINE_DUALQUATERNION_HPP

#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
/**
 * Dual quaternions are based on the dual-numbers algebra, somewhat
 * analogous to complex numbers, but with the imaginary unit `e` defined
 * such as e*e = 0 ; and using quaternions as the non-dual and dual part.
 * \note A unit dual quaternion q (i.e.\ |q| = 1) represents a rigid transformation.
 *       (rotation + translation).
 *
 * A good reference:
 * http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/other/dualQuaternion/index.htm
 */
class DualQuaternion {

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /**
     * Construct an uninitialized dual quaternion.
     */
    inline DualQuaternion() {}

    /**
     * Construct a dual-quaternion from two quaternions.
     */
    inline DualQuaternion( const Quaternion& q0, const Quaternion& qe ) : m_q0( q0 ), m_qe( qe ) {}

    /**
     * Construct a dual-quaternion from a rigid transform.
     * Any non-rigid component (e.g.\ scale and shear) will be ignored.
     */
    inline DualQuaternion( const Core::Transform& tr );

    DualQuaternion( const DualQuaternion& other ) = default;

    DualQuaternion& operator=( const DualQuaternion& ) = default;

    /// \name Component Access
    /// \{

    /**
     * Return the non-dual part quaternion, i.e.\ rotation part.
     */
    inline const Quaternion& getQ0() const;

    /**
     * Set the non-dual part quaternion, i.e.\ rotation part.
     */
    inline void setQ0( const Quaternion& q0 );

    /**
     * Return the dual part quaternion, i.e.\ translation part.
     */
    inline const Quaternion& getQe() const;

    /**
     * Set the dual part quaternion, i.e.\ translation part.
     */
    inline void setQe( const Quaternion& qe );
    /// \}

    /// \name Arithmetics
    /// \{

    /**
     * Return the sum of *this and \p other.
     */
    inline DualQuaternion operator+( const DualQuaternion& other ) const;

    /**
     * Return the multiplication of *this by \p scalar.
     */
    inline DualQuaternion operator*( Scalar scalar ) const;

    /**
     * Return *this after performing the sum of *this and \p other.
     */
    inline DualQuaternion& operator+=( const DualQuaternion& other );

    /**
     * Return *this after performing the multiplication of *this by \p scalar.
     */
    inline DualQuaternion& operator*=( Scalar scalar );

    /**
     * Normalize the quaternion with the dual-number norm (divides q0 and qe
     * by q0's norm).
     * \warning Will assert if the norm is zero in debug builds.
     */
    inline void normalize();
    /// \}

    /// \name Transformations
    /// \{

    /**
     * Set the dual-quaternion from a rigid transform.
     * Any non-rigid component (e.g.\ scale and shear) will be ignored.
     */
    inline void setFromTransform( const Transform& t );

    /**
     * Return the corresponding rigid transform. Assume a unit dual quaternion.
     */
    inline Transform getTransform() const;

    /**
     * Apply the transform represented by the dual quaternion to given vector.
     * \note Equivalent to translate( rotate ( p ) ).
     * \warning Assumes a unit dual quaternion.
     */
    inline Vector3 transform( const Vector3& p ) const;

    /**
     * Apply only the rotational part of the dual quaternion to the given vector.
     */
    inline Vector3 rotate( const Vector3& p ) const;

    /**
     * Apply only the translational part of the dual quaternion to the given vector.
     */
    inline Vector3 translate( const Vector3& p ) const;
    /// \}

  private:
    /// Non-dual part (representing the rotation).
    Quaternion m_q0;

    /// Dual part (representing the translation).
    Quaternion m_qe;
};

/**
 * Pre-multiplication of dual quaternion.
 */
inline DualQuaternion operator*( Scalar scalar, const DualQuaternion& dq );

} // namespace Core
} // namespace Ra

#include <Core/Math/DualQuaternion.inl>

#endif // RADIUMENGINE_DUALQUATERNION_HPP
