#include <Engine/Entity/Entity.hpp>

#include <Core/CoreMacros.hpp>
#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Component.hpp>

namespace Ra
{

    Engine::Entity::Entity( const std::string& name )
        : Core::IndexedObject()
        , m_transform( Core::Transform::Identity() )
        , m_name( name )
    {
    }

    Engine::Entity::~Entity()
    {
    }

    void Engine::Entity::addComponent( Engine::Component* component )
    {
        std::string name = component->getName();
        std::string err;
        Core::StringUtils::stringPrintf( err, "Component \"%s\" has already been added to the entity.", name.c_str() );
        CORE_ASSERT( m_components.find( name ) == m_components.end(), err.c_str() );

        m_components.insert( ComponentByName( name, component ) );

        component->setEntity( this );
    }

    Engine::Component* Engine::Entity::getComponent( const std::string& name )
    {
        Engine::Component* comp = nullptr;

        auto it = m_components.find( name );
        if ( it != m_components.end() )
        {
            comp = it->second;
        }

        return comp;
    }

    void Engine::Entity::removeComponent( const std::string& name )
    {
        std::string err;
        Core::StringUtils::stringPrintf( err, "The component \"%s\" is not part of the entity \"%s\"",
                                         name.c_str(), m_name.c_str() );
        CORE_ASSERT( m_components.find( name ) != m_components.end(), err.c_str() );

        m_components.erase( name );
    }

    void Engine::Entity::removeComponent( Engine::Component* component )
    {
        removeComponent( component->getName() );
    }

    const std::map<std::string, Engine::Component*>& Engine::Entity::getComponentsMap() const
    {
        return m_components;
    }

    void Engine::Entity::getProperties(std::vector<EditableProperty>& entityPropsOut) const
    {
        std::lock_guard<std::mutex> lock(m_transformMutex);
        entityPropsOut.push_back(EditableProperty::position("Position",m_transform.translation()));
        entityPropsOut.push_back(EditableProperty::rotation("Rotation",Core::Quaternion(m_transform.rotation())));
    }

    void Engine::Entity::setProperty(const EditableProperty& prop)
    {
       
        switch (prop.getType())
        {
            case EditableProperty::POSITION:
            {
                CORE_ASSERT(prop.getName() == "Position", "Wrong property");
                std::lock_guard<std::mutex> lock(m_transformMutex);
                m_transform.translation() = prop.asPosition();
                break;
            }
            case EditableProperty::ROTATION:
            {
                CORE_ASSERT(prop.getName() == "Rotation", "Wrong property");
                std::lock_guard<std::mutex> lock(m_transformMutex);
                m_transform.linear() = prop.asRotation().toRotationMatrix();
                break;
            }
            default:
                CORE_ASSERT(false, "Wrong property");

        }
    }

} // namespace Ra
