#include <Engine/Managers/EntityManager/EntityManager.hpp>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>

#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {

EntityManager::EntityManager() {
    Entity* ent( SystemEntity::createInstance() );
    ent->idx = m_entities.emplace( std::move( ent ) );
    CORE_ASSERT( ent == SystemEntity::getInstance(), "Invalid singleton instanciation" );
    m_entitiesName.insert( std::pair<std::string, Core::Index>( ent->getName(), ent->idx ) );
    RadiumEngine::getInstance()->getSignalManager()->fireEntityCreated(
        ItemEntry( SystemEntity::getInstance() ) );
}

EntityManager::~EntityManager() {}

Entity* EntityManager::createEntity( const std::string& name ) {
    Core::Index idx = m_entities.emplace( new Entity( name ) );
    auto& ent = m_entities[idx];
    ent->idx = idx;

    std::string entityName = name;
    if ( name == "" )
    {
        Core::StringUtils::stringPrintf( entityName, "Entity_%u", idx.getValue() );
        ent->rename( entityName );
    } else
    {
        int i=1;
        bool mustRename = false;
        while ( entityExists( entityName ) )
        {
            LOG( logWARNING ) << "Entity `" << entityName << "` already exists";
            entityName = name + "_" + std::to_string( i++ );
            mustRename = true;
        }
        if ( mustRename )
        {
            ent->rename( entityName );
        }
    }

    m_entitiesName.insert( std::pair<std::string, Core::Index>( ent->getName(), idx ) );

    RadiumEngine::getInstance()->getSignalManager()->fireEntityCreated( ItemEntry( ent.get() ) );
    return ent.get();
}

bool EntityManager::entityExists( const std::string& name ) const {
    return m_entitiesName.find( name ) != m_entitiesName.end();
}

void EntityManager::removeEntity( Core::Index idx ) {
    CORE_ASSERT( idx.isValid() && m_entities.contains( idx ),
                 "Trying to remove an entity that has not been added to the manager." );

    auto& ent = m_entities[idx];
    std::string name = ent->getName();
    m_entities.remove( idx );
    m_entitiesName.erase( name );
}

void EntityManager::removeEntity( Entity* entity ) {
    removeEntity( entity->idx );
}

Entity* EntityManager::getEntity( Core::Index idx ) const {
    CORE_ASSERT( idx.isValid(), "Trying to access an invalid component." );

    Entity* ent = nullptr;

    if ( m_entities.contains( idx ) )
    {
        ent = m_entities[idx].get();
    }

    return ent;
}

std::vector<Entity*> EntityManager::getEntities() const {
    std::vector<Entity*> entities;
    entities.resize( m_entities.size() );

    std::transform( m_entities.begin(), m_entities.end(), entities.begin(),
                    [](const auto &e){ return e.get(); });

    return entities;
}

Entity* EntityManager::getEntity( const std::string& name ) const {
    Entity* ent = nullptr;
    auto idx = m_entitiesName.find( name );
    CORE_ASSERT( idx != m_entitiesName.end(),
                 "Trying to access an invalid entity (named: " + name + ")" );
    ent = m_entities.at( idx->second ).get();
    return ent;
}

void EntityManager::swapBuffers() {
    for ( auto& e : m_entities )
    {
        e->swapTransformBuffers();
    }
}

void EntityManager::deleteEntities() {
    std::vector<uint> indices;
    indices.reserve( m_entities.size() - 1 );
    for ( uint i = 1; i < m_entities.size(); ++i )
    {
        indices.push_back( m_entities.index( i ) );
    }
    for ( const auto& idx : indices )
    {
        removeEntity( idx );
    }
}
} // namespace Engine
} // namespace Ra
