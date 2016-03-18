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

        inline DualQuaternion DualQuaternion::operator+ ( const DualQuaternion& other ) const
        {
            return DualQuaternion( m_q0 + other.m_q0, m_qe + other.m_qe );
        }

        inline DualQuaternion DualQuaternion::operator* ( Scalar scalar ) const
        {
            return DualQuaternion( scalar * m_q0, scalar * m_qe );
        }

        inline DualQuaternion& DualQuaternion::operator+= ( const DualQuaternion& other )
        {
            *this = *this + other;
            return *this;
        }

        inline DualQuaternion& DualQuaternion::operator*= ( Scalar scalar )
        {
            *this = *this * scalar;
            return *this;
        }

        inline void DualQuaternion::normalize()
        {
            const Scalar norm = m_q0.norm();
            CORE_ASSERT(norm != 0, "Normalizing a null quaternion.");
            m_q0 = m_q0 / norm;
            m_qe = m_qe / norm;
        }

        inline Vector3 DualQuaternion::transform(const Vector3& p) const
        {
            CORE_ASSERT( Ra::Core::Math::areApproxEqual(m_q0.norm(), 1.f), "Dual quaternion not normalized");

            // Translation from the normalized dual quaternion equals :
            // 2 * q_e * conjugate(q_0)
            Vector3 v0 = m_q0.vec();
            Vector3 ve = m_qe.vec();
            auto trans = (ve * m_q0.w() - v0 * m_qe.w() + v0.cross(ve)) * 2.f;

            // Rotate and return the result.
            return m_q0.toRotationMatrix() * p + trans;
        }

        inline DualQuaternion::DualQuaternion(const Core::Transform& tr)
        {
            setFromTransform(tr);
        }

        inline Transform DualQuaternion::getTransform() const
        {
            // Assume the dual quat is normalized.
            CORE_ASSERT( Ra::Core::Math::areApproxEqual(m_q0.norm(), 1.f), "Dual quaternion not normalized");

            Transform result;
            result.linear() = m_q0.toRotationMatrix();
            result.translation() = (2.f * m_q0 * m_qe.conjugate()).vec();
            return result;
        }

        inline void DualQuaternion::setFromTransform(const Transform& t)
        {
            m_q0 = Quaternion(t.rotation());
            if (m_q0.w() < 0 )
            {
                m_q0.coeffs() =  -m_q0.coeffs(); // enforce positive W for linear blending.
            }
            Core::Vector4 trans = Core::Vector4::Zero();
            trans.head<3>() = t.translation();
            m_qe = 0.5f * Quaternion(trans) * m_q0;
        }

        inline DualQuaternion operator*(Scalar scalar, const DualQuaternion& dq)
        {
            return dq * scalar;
        }

    }
}
