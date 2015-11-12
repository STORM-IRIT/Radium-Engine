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

        inline DualQuaternion DualQuaternion::operator* ( Scalar scalar )
        {
            return DualQuaternion( scalar * getQ0(), scalar * getQe() );
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
            const float norm = m_q0.norm();
            m_q0 = m_q0 / norm;
            m_qe = m_qe / norm;
        }
		
		inline Vector3 DualQuaternion::transform(const Vector3& p) const
	    {
	        // As the dual quaternions may be the results from a
	        // linear blending we have to normalize it :
	        float norm = m_q0.norm();
	        Quaternion qblend_0 = m_q0 / norm;
	        Quaternion qblend_e = m_qe / norm;
	
	        // Translation from the normalized dual quaternion equals :
	        // 2.f * qblend_e * conjugate(qblend_0)
	        Vector3 v0 = qblend_0.vec();
	        Vector3 ve = qblend_e.vec();
	        Vector3 trans = (ve*qblend_0.w() - v0*qblend_e.w() + v0.cross(ve)) * 2.f;
	
	        // Rotate
	        //return qblend_0 * p + trans;
            return qblend_0.toRotationMatrix() * p + trans;
	    }

        inline DualQuaternion::DualQuaternion(const Core::Transform& tr)
        {
            setFromTransform(tr);
        }

        inline Transform DualQuaternion::getTransform() const
        {
            // Assume the dual quat is normalized.
            CORE_ASSERT(std::abs(m_q0.norm() - 1.f) <= std::numeric_limits<Scalar>::epsilon(),
            "Only a normalized dual quaternion represents a transform.");

            Transform result;
            result.linear() = m_q0.toRotationMatrix();
            result.translation() = (2.f * m_q0 * m_qe.conjugate()).vec();
            return result;
        }

        inline void DualQuaternion::setFromTransform(const Transform& t)
        {
            m_q0 = t.rotation();
            Core::Vector4 trans = Core::Vector4::Zero();
            trans.head<3>() = t.translation();
            m_qe = 0.5 * (Quaternion(trans) * m_q0);
        }
    }
}
