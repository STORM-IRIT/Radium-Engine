#include <Engine/Entity/Entity.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Engine
    {

        inline const std::string& Entity::getName() const
        {
            return m_name;
        }

        inline void Entity::rename( const std::string& name )
        {
            m_name = name;
        }

        inline void Entity::setTransform( const Core::Transform& transform )
        {
            if ( m_transformChanged )
            {
                LOG( logWARNING ) << "This entity transform has already been set during this frame, ignored.";
                return;
            }
            m_transformChanged = true;
            m_doubleBufferedTransform = transform;
        }

        inline void Entity::setTransform( const Core::Matrix4& transform )
        {
            setTransform( Core::Transform( transform ));
        }

        inline Core::Transform Entity::getTransform() const
        {
            std::lock_guard<std::mutex> lock( m_transformMutex );
            return m_transform;
        }

        inline Core::Matrix4 Entity::getTransformAsMatrix() const
        {
            std::lock_guard<std::mutex> lock( m_transformMutex );
            return m_transform.matrix();
        }

        inline uint Entity::getNumComponents() const
        {
            return uint(m_components.size());
        }
    }

} // namespace Ra
