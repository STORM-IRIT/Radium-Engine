#include <Engine/Entity/EntityManager.hpp>

#include <Core/String/StringUtils.hpp>
#include <Engine/Entity/Entity.hpp>

namespace Ra
{

Engine::EntityManager::EntityManager()
{
}

Engine::EntityManager::~EntityManager()
{
    for (uint i = 0; i < m_entities.size(); ++i)
    {
        auto ent = m_entities[i];
        ent.reset();
    }

    m_entitiesName.clear();
    m_entities.clear();
}

Engine::Entity* Engine::EntityManager::getOrCreateEntity(const std::string &name)
{
    if (entityExists(name))
    {
        return getEntity(name);
    }

    return createEntity(name);
}

Engine::Entity* Engine::EntityManager::createEntity()
{
    std::shared_ptr<Engine::Entity> ent(new Entity());
    ent->idx = m_entities.insert(ent);

    std::string name;
    Core::StringUtils::stringPrintf(name, "Entity_%u", ent->idx.getValue());
    ent->rename(name);

    m_entitiesName.insert(std::pair<std::string, Core::Index>(
        ent->getName(), ent->idx));

    return ent.get();
}

bool Engine::EntityManager::entityExists(const std::string& name) const
{
    if (m_entitiesName.find(name) != m_entitiesName.end())
    {
        return true;
    }

    return false;
}

void Engine::EntityManager::removeEntity(Core::Index idx)
{
    CORE_ASSERT(idx != Core::Index::INVALID_IDX() && m_entities.contain(idx),
                "Trying to remove an entity that has not been added to the manager.");

    auto ent = m_entities[idx];
    std::string name = ent->getName();

    CORE_ASSERT(ent.unique(), "Non-unique entity about to be removed.");

    ent.reset();
    m_entities.remove(idx);
    m_entitiesName.erase(name);
}

void Engine::EntityManager::removeEntity(Engine::Entity* entity)
{
    removeEntity(entity->idx);
}

Engine::Entity* Engine::EntityManager::getEntity(Core::Index idx) const
{
    CORE_ASSERT(idx != Core::Index::INVALID_IDX(), "Trying to access an invalid component.");

    Engine::Entity* ent = nullptr;

    if (m_entities.contain(idx))
    {
        ent = m_entities.at(idx).get();
    }

    return ent;
}

std::vector<Engine::Entity*> Engine::EntityManager::getEntities() const
{
    std::vector<Engine::Entity*> entities;
    uint size = m_entities.size();

    entities.reserve(size);

    for (uint i = 0; i < size; ++i)
    {
        entities.push_back(m_entities.at(i).get());
    }

    return entities;
}

Engine::Entity* Engine::EntityManager::createEntity(const std::string& name)
{
    std::shared_ptr<Engine::Entity> ent = std::shared_ptr<Engine::Entity>(new Engine::Entity(name));
    ent->idx = m_entities.insert(ent);

    m_entitiesName.insert(std::pair<std::string, Core::Index>(
        ent->getName(), ent->idx));

    return ent.get();
}

Engine::Entity* Engine::EntityManager::getEntity(const std::string& name) const
{
    return m_entities.at(m_entitiesName.at(name)).get();
}


} // namespace Ra
