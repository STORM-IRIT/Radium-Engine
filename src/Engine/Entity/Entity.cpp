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

            RadiumEngine::getInstance()->getSignalManager()->fireComponentAdded( ItemEntry(this, component) );

        }

        Component* Entity::getComponent( const std::string& name )
        {
            const auto& pos = std::find_if(
                        m_components.begin(),
                        m_components.end(),
                        [ name ] (const auto& c){ return c->getName() == name;} );

            return pos != m_components.end()? pos->get() : nullptr;
        }

        const Component* Entity::getComponent(const std::string& name) const
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
            RadiumEngine::getInstance()->getSignalManager()->fireComponentRemoved(ItemEntry(this,pos->get()));
            m_components.erase(pos);
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
