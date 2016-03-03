#include <Engine/System/System.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Component/Component.hpp>
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
        }

        void System::registerComponent( const Entity* ent,  Component* component )
        {
            // Perform checks on debug
#if defined(DEBUG)
            CORE_ASSERT( component->getEntity() == ent, "Component does not belong to entity" );
            for (const auto& pair : m_components)
            {
                CORE_ASSERT(pair.second != component, "Component already registered");
                if (pair.first == ent)
                {
                    CORE_ASSERT(pair.second->getName() != component->getName(),
                        "A component with the same name is already associated with this entity");
                }
            }
#endif // DEBUG
            m_components.push_back({ ent, component });
            component->setSystem( this );

        }

        void System::unregisterComponent( const Entity* ent, Component* component )
        {
            CORE_ASSERT( component->getEntity() == ent, "Component does not belong to entity" );
            const auto& pos =
                std::find_if(m_components.begin(), m_components.end(),
                [component](const auto& pair) { return pair.second == component; });

            CORE_ASSERT( pos != m_components.end(), "Component is not registered." );
            CORE_ASSERT( pos->first == ent, "Component belongs to a different entity" );

            m_components.erase( pos );
        }


        void System::unregisterAllComponents( const Entity* entity )
        {
            std::vector<std::pair<const Entity*, Component*>>::iterator pos;
            while ( (pos = std::find_if( m_components.begin(), m_components.end(),
                [entity]( const auto& pair ) {return pair.first == entity; } )) != m_components.end())
            {
                m_components.erase( pos );
            }
        }
    }
} // namespace Ra
