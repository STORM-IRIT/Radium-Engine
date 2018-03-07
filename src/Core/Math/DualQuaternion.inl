#include "DualQuaternion.hpp"

namespace Ra {
namespace Core {

inline const Quaternion& DualQuaternion::getQ0() const {
    return m_q0;
}

inline void DualQuaternion::setQ0( const Quaternion& q0 ) {
    m_q0 = q0;
}

inline const Quaternion& DualQuaternion::getQe() const {
    return m_qe;
}

inline void DualQuaternion::setQe( const Quaternion& qe ) {
    m_qe = qe;
}

inline DualQuaternion DualQuaternion::operator+( const DualQuaternion& other ) const {
    return DualQuaternion( m_q0 + other.m_q0, m_qe + other.m_qe );
}

inline DualQuaternion DualQuaternion::operator*( Scalar scalar ) const {
    return DualQuaternion( scalar * m_q0, scalar * m_qe );
}

inline DualQuaternion& DualQuaternion::operator+=( const DualQuaternion& other ) {
    *this = *this + other;
    return *this;
}

inline DualQuaternion& DualQuaternion::operator*=( Scalar scalar ) {
    *this = *this * scalar;
    return *this;
}

inline void DualQuaternion::normalize() {
    const Scalar norm = m_q0.norm();
    CORE_ASSERT( norm != 0, "Normalizing a null quaternion." );
    m_q0 = m_q0 / norm;
    m_qe = m_qe / norm;
}

inline Vector3 DualQuaternion::transform( const Vector3& p ) const {
    CORE_ASSERT( Ra::Core::Math::areApproxEqual( m_q0.norm(), 1.f ),
                 "Dual quaternion not normalized" );
    return translate( rotate( p ) );
}

inline Vector3 DualQuaternion::rotate( const Vector3& p ) const {
    CORE_ASSERT( Ra::Core::Math::areApproxEqual( m_q0.norm(), 1.f ),
                 "Dual quaternion not normalized" );
    return m_q0.toRotationMatrix() * p;
}

inline Vector3 DualQuaternion::translate( const Vector3& p ) const {
    Vector3 v0 = m_q0.vec();
    Vector3 ve = m_qe.vec();
    return p + ( ( ve * m_q0.w() - v0 * m_qe.w() + v0.cross( ve ) ) * 2.f );
}

inline DualQuaternion::DualQuaternion( const Core::Transform& tr ) {
    setFromTransform( tr );
}

inline Transform DualQuaternion::getTransform() const {
    // Assume the dual quat is normalized.
    CORE_ASSERT( Ra::Core::Math::areApproxEqual( m_q0.norm(), 1.f ),
                 "Dual quaternion not normalized" );

    Transform result;
    result.linear() = m_q0.toRotationMatrix();
    result.translation() = ( 2.f * m_q0 * m_qe.conjugate() ).vec();
    return result;
}

inline void DualQuaternion::setFromTransform( const Transform& t ) {
    m_q0 = Quaternion( t.rotation() );
    Core::Vector4 trans = Core::Vector4::Zero();
    trans.head<3>() = t.translation();
    m_qe = 0.5f * Quaternion( trans ) * m_q0;
}

inline DualQuaternion operator*( Scalar scalar, const DualQuaternion& dq ) {
    return dq * scalar;
}

} // namespace Core
} // namespace Ra
