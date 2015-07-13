#include <Engine/Entity/EntityManager.hpp>

#include <Engine/Entity/Entity.hpp>

namespace Ra
{

EntityManager::~EntityManager()
{
    for (int i = 0; i < m_entities.size(); ++i)
    {
        // FIXME (Charly): Check if every accessed idx is guaranted to exist.
        auto ent = m_entities[i];
        CORE_ASSERT(ent.unique(), "Non-unique entity about to be removed.");
        ent.reset();
    }

    m_entities.clear();
}

Entity* EntityManager::createEntity()
{
    std::shared_ptr<Entity> ent = std::make_shared<Entity>();
    ent->idx = m_entities.insert(ent);
    return ent.get();
}

void EntityManager::removeEntity(Index idx)
{
    CORE_ASSERT(idx != Index::INVALID_IDX() && m_entities.contain(idx),
                "Trying to remove an entity that has not been added to the manager.");

    auto ent = m_entities[idx];

    CORE_ASSERT(ent.unique(), "Non-unique entity about to be removed.");

    ent.reset();
    m_entities.remove(idx);
}

void EntityManager::removeEntity(Entity* entity)
{
    removeEntity(entity->idx);
}

Entity* EntityManager::getEntity(Index idx) const
{
    CORE_ASSERT(idx != Index::INVALID_IDX(), "Trying to access an invalid component.");

    Entity* ent = nullptr;

    if (m_entities.contain(idx))
    {
        ent = m_entities.at(idx).get();
    }

    return ent;
}

std::vector<Entity*> EntityManager::getEntities() const
{
    std::vector<Entity*> entities;
    uint size = m_entities.size();

    entities.reserve(size);

    for (int i = 0; i < size; ++i)
    {
        entities.push_back(m_entities.at(i).get());
    }

    return entities;
}

} // namespace Ra
