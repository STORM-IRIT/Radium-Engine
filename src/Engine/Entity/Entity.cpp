#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Component.hpp>

#include <Engine/Entity/System.hpp>

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

        Entity::~Entity()
        {
            for ( const auto& component : m_components )
            {
                System* system = component.second->getSystem();
                if ( system != nullptr )
                {
                    system->removeComponent( component.first );
                }
            }
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

            auto it = m_components.find( name );

            CORE_ASSERT( it != m_components.end(), err.c_str());

            if ( it != m_components.end() )
            {
                System* system = it->second->getSystem();
                if ( system != nullptr )
                {
                    system->removeComponent( it->first );
                }
                m_components.erase( it );
            }
            else
            {
                LOG( logERROR ) << "Trying to remove component " << name
                                << " which is not attached to the entity.";
            }
        }

        void Entity::removeComponent( Engine::Component* component )
        {
            removeComponent( component->getName() );
        }

        const std::map<std::string, Engine::Component*>& Engine::Entity::getComponentsMap() const
        {
            return m_components;
        }

        void Entity::getProperties( Core::AlignedStdVector<EditableProperty>& entityPropsOut ) const
        {
            std::lock_guard<std::mutex> lock( m_transformMutex );
            entityPropsOut.push_back(EditableProperty(m_transform, "Entity Transform", true, true));
        }

        void Entity::setProperty( const EditableProperty& prop )
        {
            CORE_ASSERT(prop.type == EditableProperty::TRANSFORM, "Only transforms can be set in entities");
            CORE_ASSERT(prop.name == "Entity Transform", "This is not an entity transform");

            for(const auto& entry: prop.primitives)
            {
                const EditablePrimitive& prim = entry.primitive;
                switch (prim.getType())
                {
                    case EditablePrimitive::POSITION:
                    {
                        CORE_ASSERT(prim.getName() == "Position", "Inconsistent primitive");
                        m_doubleBufferedTransform.translation() =
                                prim.asPosition();
                        m_transformChanged = true;
                    }
                        break;

                    case EditablePrimitive::ROTATION:
                    {
                        CORE_ASSERT(prim.getName() == "Rotation", "Inconsistent primitive");
                        m_doubleBufferedTransform.linear() = //FIXME : this won't work with scale.
                                prim.asRotation().toRotationMatrix();
                        m_transformChanged = true;
                    }
                        break;

                    default:
                    {
                        CORE_ASSERT(false, "Wrong primitive type in property");
                    }
                        break;
                }
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
