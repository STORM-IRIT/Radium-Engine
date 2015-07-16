#include <Engine/Entity/EntityManager.hpp>

#include <Engine/Entity/Entity.hpp>

namespace Ra
{

Engine::EntityManager::~EntityManager()
{
    for (int i = 0; i < m_entities.size(); ++i)
    {
        auto ent = m_entities[i];
        CORE_ASSERT(ent.unique(), "Non-unique entity about to be removed.");
        ent.reset();
    }

    m_entities.clear();
}

Engine::Entity* Engine::EntityManager::createEntity()
{
    std::shared_ptr<Engine::Entity> ent = std::make_shared<Engine::Entity>();
    ent->idx = m_entities.insert(ent);
    return ent.get();
}

void Engine::EntityManager::removeEntity(Core::Index idx)
{
    CORE_ASSERT(idx != Core::Index::INVALID_IDX() && m_entities.contain(idx),
                "Trying to remove an entity that has not been added to the manager.");

    auto ent = m_entities[idx];

    CORE_ASSERT(ent.unique(), "Non-unique entity about to be removed.");

    ent.reset();
    m_entities.remove(idx);
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

    for (int i = 0; i < size; ++i)
    {
        entities.push_back(m_entities.at(i).get());
    }

    return entities;
}

} // namespace Ra
