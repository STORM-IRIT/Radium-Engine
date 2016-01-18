#include <Engine/Entity/System.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>

namespace Ra
{
    namespace Engine
    {
        System::System()
        {
        }

        System::~System()
        {
            // Should be already cleared, but just in case ...
            for ( auto& component : m_components )
            {
                component.second.reset();
            }
        }

        void System::addComponent( Component* component )
        {
            std::shared_ptr<Component> comp( component );
            std::string name = component->getName();

            std::string err;
            Core::StringUtils::stringPrintf( err, "The component \"%s\" has already been added to the system.",
                                             name.c_str());
            CORE_ASSERT( m_components.find( name ) == m_components.end(), err.c_str());

            m_components[name] = comp;

            component->setSystem( this );
        }

        void System::removeComponent( const std::string& name )
        {
            auto it = m_components.find( name );
            if ( it != m_components.end() )
            {
                std::shared_ptr<Component> component = m_components[name];
                component.reset();
                m_components.erase( name );
            }
            else
            {
                LOG( logERROR ) << "Trying to remove component " << name
                                << " which is not attached to the system.";
            }
        }

        void System::removeComponent( Engine::Component* component )
        {
            removeComponent( component->getName());
        }

        Component* System::addComponentToEntity( Entity* entity )
        {
            uint id = entity->getNumComponents();
            Component* component = addComponentToEntityInternal( entity, id );

            component->setEntity( entity );

            entity->addComponent( component );
            this->addComponent( component );

            component->initialize();

            return component;
        }

        void System::registerOnComponentCreation(Ra::Engine::System* system)
        {
            m_dependentSystems.push_back(system);
        }

        void System::callOnComponentCreationDependencies(const Ra::Engine::Component* component)
        {
            for (System* dependency : m_dependentSystems)
                dependency->callbackOnComponentCreation(component);
        }


        void System::manageDependencyOfComponent() {
            for( const auto& component : m_components ) {
                callOnComponentCreationDependencies( component.second.get() );
            }
        }
    }
} // namespace Ra
