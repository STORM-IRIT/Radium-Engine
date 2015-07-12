#ifndef RADIUMENGINE_DUALQUATERNION_HPP
#define RADIUMENGINE_DUALQUATERNION_HPP

#include <Core/Math/Vector.hpp>

namespace Ra
{
    /// Dual quaternions are based on the dual-numbers algebra, somewhat
    /// analogous to complex numbers, but with the imaginary unit `e` defined
    /// such as e*e = 0 ; and using quaternions as the non-dual and dual part.
    /// Unit dual quaternions can represent any rigid transformation
    /// (rotation + translation).
    class DualQuaternion
    {

    public:
        // Construct an uninitialized dual quaternion.
        DualQuaternion() {}

        /// Construct a dual-quaternion from two quaternions.
        DualQuaternion(const Quaternion& q0, const Quaternion& qe) : m_q0(q0), m_qe(qe) { }

        /// Default copy constructor and assignment operator.
        DualQuaternion (const DualQuaternion& other) = default;
        DualQuaternion& operator= ( const DualQuaternion& ) = default;

        /// todo : conversion to/from Transform

        /// Getters and setters
        inline const Quaternion& getQ0() const;
        inline void setQ0(const Quaternion& q0);
        inline const Quaternion& getQe() const;
        inline void setQe(const Quaternion& qe);

        /// Operators

        inline DualQuaternion operator+ ( const DualQuaternion& other);
        inline DualQuaternion operator* ( Scalar scalar );

        inline DualQuaternion& operator +=( const DualQuaternion& other);
        inline DualQuaternion& operator *=( Scalar scalar );

        /// Other methods

        /// Normalize the quaternion with the dual-number norm (divides q0 and qe
        /// by q0's norm).
        inline void normalize();

    private:
        /// non-dual part
        Quaternion m_q0;
        /// dual part
        Quaternion m_qe;
    };
}

#include <Core/Math/DualQuaternion.inl>

#endif //RADIUMENGINE_DUALQUATERNION_HPP
