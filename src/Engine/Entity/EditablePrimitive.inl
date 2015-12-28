#include "EditablePrimitive.hpp"
namespace Ra
{
    namespace Engine
    {

        inline EditablePrimitive::EditablePrimitive( const EditablePrimitive& other )
        {
            *this = other;
        }

        inline EditablePrimitive& EditablePrimitive::operator=( const EditablePrimitive& other )
        {
            m_type = other.m_type;
            // We could copy the union member according to the type but this is way simpler.
            memcpy( &m_value, &other.m_value, sizeof( m_value ) );
            m_name = other.m_name;
            return *this;
        }

        inline EditablePrimitive EditablePrimitive::color( const std::string& name,
                                                         const Core::Color& color /* = Core::Color:Ones()*/ )
        {
            EditablePrimitive result;
            result.m_value.color = color;
            result.m_type = COLOR;
            result.m_name = name;
            return result;
        }

        inline EditablePrimitive EditablePrimitive::rotation( const std::string& name,
                                                            const Core::Quaternion& quat /*= Core::Quaternion::Identity()*/ )
        {
            EditablePrimitive result;
            result.m_value.rotation = quat;
            result.m_type = ROTATION;
            result.m_name = name;
            return result;
        }

        inline EditablePrimitive EditablePrimitive::scalar( const std::string& name, Scalar scalar /*= 0.f*/ )
        {
            EditablePrimitive result;
            result.m_value.scalar = scalar;
            result.m_type = SCALAR;
            result.m_name = name;
            return result;
        }

        inline EditablePrimitive EditablePrimitive::scale( const std::string& name,
                                                         const Core::Vector3& scale /*= Core::Vector3::Ones()*/ )
        {
            EditablePrimitive result;
            result.m_value.scale = scale;
            result.m_type = SCALE;
            result.m_name = name;
            return result;
        }

        inline EditablePrimitive EditablePrimitive::position( const std::string& name,
                                                            const Core::Vector3& vector /*= Core::Vector3::Zero()*/ )
        {
            EditablePrimitive result;
            result.m_value.position = vector;
            result.m_type = POSITION;
            result.m_name = name;
            return result;
        }

        inline EditablePrimitive::Type EditablePrimitive::getType() const
        {
            return m_type;
        }

        const std::string& EditablePrimitive::getName() const
        {
            return m_name;
        }

        // non-const versions

        inline Scalar& EditablePrimitive::asScalar()
        {
            CORE_ASSERT( m_type == SCALAR, "Wrong type" );
            return m_value.scalar;
        }

        inline Core::Vector3& EditablePrimitive::asPosition()
        {
            CORE_ASSERT( m_type == POSITION, "Wrong type" );
            return m_value.position;
        }

        inline Core::Quaternion& EditablePrimitive::asRotation()
        {
            CORE_ASSERT( m_type == ROTATION, "Wrong type" );
            return m_value.rotation;
        }

        inline Core::Vector3& EditablePrimitive::asScale()
        {
            CORE_ASSERT( m_type == SCALE, "Wrong type" );
            return m_value.scale;
        }

        inline Core::Color& EditablePrimitive::asColor()
        {
            CORE_ASSERT( m_type == COLOR, "Wrong type" );
            return m_value.color;
        }

        // const versions

        inline const Scalar& EditablePrimitive::asScalar() const
        {
            CORE_ASSERT( m_type == SCALAR, "Wrong type" );
            return m_value.scalar;
        }

        inline const Core::Vector3& EditablePrimitive::asPosition() const
        {
            CORE_ASSERT( m_type == POSITION, "Wrong type" );
            return m_value.position;
        }

        inline const Core::Quaternion& EditablePrimitive::asRotation() const
        {
            CORE_ASSERT( m_type == ROTATION, "Wrong type" );
            return m_value.rotation;
        }

        inline const Core::Vector3& EditablePrimitive::asScale() const
        {
            CORE_ASSERT( m_type == SCALE, "Wrong type" );
            return m_value.scale;
        }

        inline const Core::Color& EditablePrimitive::asColor() const
        {
            CORE_ASSERT( m_type == COLOR, "Wrong type" );
            return m_value.color;
        }


    }
}