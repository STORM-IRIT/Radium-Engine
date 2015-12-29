#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Math/Ray.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/System/System.hpp>

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
            // Assert if I've been removed from ent mgr ?
        }

        void Entity::addComponent( Engine::Component* component )
        {
            std::string name = component->getName();
            std::string err;
            Core::StringUtils::stringPrintf( err, "Component \"%s\" has already been added to the entity.",
                                             name.c_str());
            CORE_ASSERT( getComponent(name) == nullptr, err.c_str());

            m_components.emplace_back( std::unique_ptr<Component>(component));
            component->setEntity( this );

            RadiumEngine::getInstance()->getSignalManager()->fireComponentAdded();

        }

        Component* Entity::getComponent( const std::string& name )
        {
            const auto& pos = std::find_if(
                        m_components.begin(),
                        m_components.end(),
                        [ name ] (const auto& c){ return c->getName() == name;} );


            return pos != m_components.end()? pos->get() : nullptr;
        }

        const std::vector<std::unique_ptr<Component> > &Entity::getComponents() const
        {
           return m_components;
        }

        void Entity::removeComponent( const std::string& name )
        {
            const auto& pos = std::find_if(
                        m_components.begin(),
                        m_components.end(),
                        [ name ] (const auto& c){ return c->getName() == name;} );

            CORE_ASSERT( pos != m_components.end(), "Component not found in entity" );
            m_components.erase(pos);
            RadiumEngine::getInstance()->getSignalManager()->fireComponentRemoved();
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

        void Entity::rayCastQuery(const Core::Ray& r) const
        {
            // put ray in local frame.
            Core::Ray transformedRay = Ra::Core::transformRay(r, m_transform.inverse());
            for (const auto& c : m_components)
            {
                c->rayCastQuery(transformedRay);
            }
        }

    }

} // namespace Ra
