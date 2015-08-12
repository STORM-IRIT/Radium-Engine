#include <Engine/Entity/EntityManager.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Entity.hpp>

namespace Ra
{
    namespace Engine
    {

        EntityManager::EntityManager()
        {
        }

        EntityManager::~EntityManager()
        {
            for ( uint i = 0; i < m_entities.size(); ++i )
            {
                auto ent = m_entities[i];
                ent.reset();
            }

            m_entitiesName.clear();
            m_entities.clear();
        }

        Entity* EntityManager::getOrCreateEntity( const std::string& name )
        {
            if ( entityExists( name ) )
            {
                return getEntity( name );
            }

            return createEntity( name );
        }

        Entity* EntityManager::createEntity()
        {
            std::shared_ptr<Entity> ent( new Entity() );
            ent->idx = m_entities.insert( ent );

            std::string name;
            Core::StringUtils::stringPrintf( name, "Entity_%u", ent->idx.getValue() );
            ent->rename( name );

            m_entitiesName.insert( std::pair<std::string, Core::Index> (
                                       ent->getName(), ent->idx ) );

            return ent.get();
        }

        bool EntityManager::entityExists( const std::string& name ) const
        {
            if ( m_entitiesName.find( name ) != m_entitiesName.end() )
            {
                return true;
            }

            return false;
        }

        void EntityManager::removeEntity( Core::Index idx )
        {
            CORE_ASSERT( idx != Core::Index::INVALID_IDX() && m_entities.contain( idx ),
                         "Trying to remove an entity that has not been added to the manager." );

            auto ent = m_entities[idx];
            std::string name = ent->getName();

            CORE_ASSERT( ent.unique(), "Non-unique entity about to be removed." );

            ent.reset();
            m_entities.remove( idx );
            m_entitiesName.erase( name );
        }

        void EntityManager::removeEntity( Entity* entity )
        {
            removeEntity( entity->idx );
        }

        Entity* EntityManager::getEntity( Core::Index idx ) const
        {
            CORE_ASSERT( idx != Core::Index::INVALID_IDX(), "Trying to access an invalid component." );

            Entity* ent = nullptr;

            if ( m_entities.contain( idx ) )
            {
                ent = m_entities.at( idx ).get();
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
                entities.push_back( m_entities.at( i ).get() );
            }

            return entities;
        }

        Entity* EntityManager::createEntity( const std::string& name )
        {
            std::shared_ptr<Entity> ent = std::shared_ptr<Entity> ( new Entity( name ) );
            ent->idx = m_entities.insert( ent );

            m_entitiesName.insert( std::pair<std::string, Core::Index> (
                                       ent->getName(), ent->idx ) );

            return ent.get();
        }

        Entity* EntityManager::getEntity( const std::string& name ) const
        {
            return m_entities.at( m_entitiesName.at( name ) ).get();
        }
    }
} // namespace Ra
