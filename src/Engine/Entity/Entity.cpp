#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{
    namespace Engine
    {

        Entity::Entity( const std::string& name )
                : Core::IndexedObject()
                , m_transform( Core::Transform::Identity())
                , m_doubleBufferedTransform( Core::Transform::Identity() )
                , m_name( name )
                , m_transformChanged( false )
        {
        }

        void Entity::addComponent( Engine::Component* component )
        {
            std::string name = component->getName();
            std::string err;
            Core::StringUtils::stringPrintf( err, "Component \"%s\" has already been added to the entity.",
                                             name.c_str());
            CORE_ASSERT( m_components.find( name ) == m_components.end(), err.c_str());

            m_components.insert( ComponentByName( name, component ));

            component->setEntity( this );
        }

        Component* Entity::getComponent( const std::string& name )
        {
            Component* comp = nullptr;

            auto it = m_components.find( name );
            if ( it != m_components.end())
            {
                comp = it->second;
            }

            return comp;
        }

        void Entity::removeComponent( const std::string& name )
        {
            std::string err;
            Core::StringUtils::stringPrintf( err, "The component \"%s\" is not part of the entity \"%s\"",
                                             name.c_str(), m_name.c_str());
            CORE_ASSERT( m_components.find( name ) != m_components.end(), err.c_str());

            m_components.erase( name );
        }

        void Entity::removeComponent( Engine::Component* component )
        {
            removeComponent( component->getName());
        }

        const std::map<std::string, Engine::Component*>& Engine::Entity::getComponentsMap() const
        {
            return m_components;
        }

        void Entity::getProperties( Core::AlignedStdVector<EditableProperty>& entityPropsOut ) const
        {
            std::lock_guard<std::mutex> lock( m_transformMutex );
            entityPropsOut.push_back( EditableProperty::position( "Position", m_transform.translation()));
            entityPropsOut.push_back(
                    EditableProperty::rotation( "Rotation", Core::Quaternion( m_transform.rotation())));
        }

        void Entity::setProperty( const EditableProperty& prop )
        {

            switch (prop.getType())
            {
                case EditableProperty::POSITION:
                {
                    CORE_ASSERT( prop.getName() == "Position", "Wrong property" );
                    m_doubleBufferedTransform.translation() =
                            prop.asPosition();
                    m_transformChanged = true;
                } break;

                case EditableProperty::ROTATION:
                {
                    CORE_ASSERT( prop.getName() == "Rotation", "Wrong property" );
                    m_doubleBufferedTransform.linear() =
                            prop.asRotation().toRotationMatrix();
                    m_transformChanged = true;
                } break;

                default:
                {
                    CORE_ASSERT( false, "Wrong property" );
                } break;
            }
        }

        void Entity::swapTransformBuffers()
        {
            if ( m_transformChanged )
            {
                m_transform = m_doubleBufferedTransform;
                m_transformChanged = false;
            }
        }
    }

} // namespace Ra
