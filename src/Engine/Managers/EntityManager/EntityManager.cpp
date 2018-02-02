#include <Engine/Managers/EntityManager/EntityManager.hpp>

#include <Core/String/StringUtils.hpp>
#include <Core/Log/Log.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>

namespace Ra
{
    namespace Engine
    {

        EntityManager::EntityManager()
        {
            Entity* ent( SystemEntity::createInstance() );
            ent->idx = m_entities.emplace( std::move(ent) );
            CORE_ASSERT( ent == SystemEntity::getInstance(), "Invalid singleton instanciation");
            m_entitiesName.insert( std::pair< std::string, Core::Index> (ent->getName(),ent->idx ));
            RadiumEngine::getInstance()->getSignalManager()->fireEntityCreated( ItemEntry(SystemEntity::getInstance()));
        }

        EntityManager::~EntityManager()
        {
        }

        Entity* EntityManager::createEntity( const std::string& name )
        {

            std::string entityName = name;
            if ( entityExists( name ) )
            {
                LOG( logWARNING ) << "Entity `" << name << "` already exists";
                entityName = name + "_";
            }
            Core::Index idx = m_entities.emplace( new Entity(entityName) );
            auto& ent = m_entities[idx];
            ent->idx = idx;

            if (name == "")
            {
                std::string name;
                Core::StringUtils::stringPrintf( name, "Entity_%u", idx.getValue() );
                ent->rename( name );
            }

            m_entitiesName.insert( std::pair<std::string, Core::Index> (
                                       ent->getName(), idx ) );

            RadiumEngine::getInstance()->getSignalManager()->fireEntityCreated(ItemEntry(ent.get()));
            return ent.get();
        }

        bool EntityManager::entityExists( const std::string& name ) const
        {
            return m_entitiesName.find( name ) != m_entitiesName.end();
        }

        void EntityManager::removeEntity( Core::Index idx )
        {
            CORE_ASSERT( idx.isValid() && m_entities.contains( idx ),
                         "Trying to remove an entity that has not been added to the manager." );

            auto& ent = m_entities[idx];
            std::string name = ent->getName();
            m_entities.remove( idx );
            m_entitiesName.erase( name );
        }

        void EntityManager::removeEntity( Entity* entity )
        {
            removeEntity( entity->idx );
        }

        Entity* EntityManager::getEntity( Core::Index idx ) const
        {
            CORE_ASSERT( idx.isValid(), "Trying to access an invalid component." );

            Entity* ent = nullptr;

            if ( m_entities.contains( idx ) )
            {
                ent = m_entities[idx].get();
            }

            return ent;
        }

        std::vector<Entity*> EntityManager::getEntities() const
        {
            std::vector<Entity*> entities;
            uint size = m_entities.size();

            entities.reserve( size );

            for ( uint i = 0; i < size; ++i )
            {
                entities.push_back( m_entities[i].get() );
            }

            return entities;
        }

        Entity* EntityManager::getEntity( const std::string& name ) const
        {
            Entity *ent = nullptr;
            auto idx = m_entitiesName.find( name );
            CORE_ASSERT( idx != m_entitiesName.end(), "Trying to access an invalid entity (named: " + name +")");
            ent = m_entities.at( idx->second ).get();
            return ent;
        }

        void EntityManager::swapBuffers()
        {
            for ( uint i = 0; i < m_entities.size(); ++i )
            {
                m_entities[i]->swapTransformBuffers();
            }
        }

        void EntityManager::deleteEntities()
        {
            std::vector<uint> indices;
            indices.reserve( m_entities.size() - 1 );
            for (uint i = 1 ; i < m_entities.size(); ++i)
            {
                indices.push_back(m_entities.index(i));
            }
            for (const auto& idx : indices)
            {
                removeEntity(idx);
            }
        }
    }
} // namespace Ra
