namespace Ra 
{

    const Quaternion& DualQuaternion::getQ0() const
    {
        return m_q0;
    }

    void DualQuaternion::setQ0(const Quaternion& q0)
    {
        m_q0 = q0;
    }

    const Quaternion& DualQuaternion::getQe() const
    {
        return m_qe;
    }

    void DualQuaternion::setQe(const Quaternion& qe)
    {
        m_qe = qe;
    }

    DualQuaternion DualQuaternion::operator+(const DualQuaternion& other)
    {
        return DualQuaternion( getQ0() + other.getQ0(), getQe() + other.getQe());
    }

    DualQuaternion DualQuaternion::operator*(Saclar scalar)
    {
        return DualQuaternion( getQ0() * scalar, getQe() * scalar);
    }

    DualQuaternion& DualQuaternion::operator+=(const DualQuaternion& other)
    {
        m_q0 += other.getQ0();
        m_qe += other.getQe();
    }

    DualQuaternion& DualQuaternion::operator*=( Scalar scalar )
    {
        m_q0 *= scalar;
        m_qe += scalar;
    }

    void DualQuaternion::normalize()
    {
        const float norm = m_q0.norm();
        m_q0.normalize();
        m_qe /= norm;
    }
}
