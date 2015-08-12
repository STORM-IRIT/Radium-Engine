#include "DualQuaternion.hpp"

namespace Ra
{
    namespace Core
    {

        inline const Quaternion& DualQuaternion::getQ0() const
        {
            return m_q0;
        }

        inline void DualQuaternion::setQ0( const Quaternion& q0 )
        {
            m_q0 = q0;
        }

        inline const Quaternion& DualQuaternion::getQe() const
        {
            return m_qe;
        }

        inline void DualQuaternion::setQe( const Quaternion& qe )
        {
            m_qe = qe;
        }

        inline DualQuaternion DualQuaternion::operator+ ( const DualQuaternion& other )
        {
            return DualQuaternion( getQ0() + other.getQ0(), getQe() + other.getQe() );
        }

        inline DualQuaternion DualQuaternion::operator* ( Ra::Scalar scalar )
        {
            return DualQuaternion( scalar * getQ0(), scalar * getQe() );
        }

        inline DualQuaternion& DualQuaternion::operator+= ( const DualQuaternion& other )
        {
            *this = *this + other;
        }

        inline DualQuaternion& DualQuaternion::operator*= ( Scalar scalar )
        {
            *this = *this * scalar;
        }

        inline void DualQuaternion::normalize()
        {
            const float norm = m_q0.norm();
            m_q0.normalize();
            m_qe = ( 1.f / norm ) * m_qe;
        }
    }
}
