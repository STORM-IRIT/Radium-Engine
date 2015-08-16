#include "EditableProperty.hpp"
namespace Ra
{
namespace Engine {

inline EditableProperty::EditableProperty(const EditableProperty& other)
{
    *this = other;
}

inline EditableProperty& EditableProperty::operator=(const EditableProperty& other)
{
    m_type = other.m_type;
    // We could copy the union member according to the type but this is way simpler.
    memcpy(&m_value, &other.m_value, sizeof(m_value));
    m_name = other.m_name;
    return *this;
}

inline EditableProperty EditableProperty::color(const std::string& name, const Core::Color& color /* = Core::Color:Ones()*/)
{
    EditableProperty result;
    result.m_value.color= color;
    result.m_type = COLOR;
    result.m_name = name;
    return result;
}

inline EditableProperty EditableProperty::rotation(const std::string& name, const Core::Quaternion& quat /*= Core::Quaternion::Identity()*/)
{
    EditableProperty result;
    result.m_value.rotation = quat;
    result.m_type = ROTATION;
    result.m_name = name;
    return result;
}

inline EditableProperty EditableProperty::scalar(const std::string& name, Scalar scalar /*= 0.f*/)
{
    EditableProperty result;
    result.m_value.scalar = scalar;
    result.m_type = SCALAR;
    result.m_name = name;
    return result;
}

inline EditableProperty EditableProperty::scale(const std::string& name, const Core::Vector3& scale /*= Core::Vector3::Ones()*/)
{
    EditableProperty result;
    result.m_value.scale = scale;
    result.m_type = SCALE;
    result.m_name = name;
    return result;
}

inline EditableProperty EditableProperty::position(const std::string& name, const Core::Vector3& vector /*= Core::Vector3::Zero()*/)
{
    EditableProperty result;
    result.m_value.position = vector;
    result.m_type = POSITION;
    result.m_name = name;
    return result;
}

inline EditableProperty::Type EditableProperty::getType() const
{
    return m_type;
}

const std::string& EditableProperty::getName() const
{
    return m_name;
}

// non-const versions

inline Scalar& EditableProperty::asScalar()
{
    CORE_ASSERT(m_type == SCALAR, "Wrong type");
    return m_value.scalar;
}

inline Core::Vector3& EditableProperty::asPosition()
{
    CORE_ASSERT(m_type == POSITION, "Wrong type");
    return m_value.position;
}

inline Core::Quaternion& EditableProperty::asRotation()
{
    CORE_ASSERT(m_type == ROTATION, "Wrong type");
    return m_value.rotation;
}

inline Core::Vector3& EditableProperty::asScale()
{
    CORE_ASSERT(m_type == SCALE, "Wrong type");
    return m_value.scale;
}

inline Core::Color& EditableProperty::asColor()
{
    CORE_ASSERT(m_type == COLOR, "Wrong type");
    return m_value.color;
}

// const versions

inline const Scalar& EditableProperty::asScalar() const
{
    CORE_ASSERT(m_type == SCALAR, "Wrong type");
    return m_value.scalar;
}

inline const Core::Vector3& EditableProperty::asPosition() const
{
    CORE_ASSERT(m_type == POSITION, "Wrong type");
    return m_value.position;
}

inline const Core::Quaternion& EditableProperty::asRotation() const
{
    CORE_ASSERT(m_type == ROTATION, "Wrong type");
    return m_value.rotation;
}

inline const Core::Vector3& EditableProperty::asScale() const
{
    CORE_ASSERT(m_type == SCALE, "Wrong type");
    return m_value.scale;
}

inline const Core::Color& EditableProperty::asColor() const
{
    CORE_ASSERT(m_type == COLOR, "Wrong type");
}


}
}