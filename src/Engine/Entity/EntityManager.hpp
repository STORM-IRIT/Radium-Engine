#ifndef RADIUMENGINE_ENTITYMANAGER_HPP
#define RADIUMENGINE_ENTITYMANAGER_HPP

#include <memory>
#include <vector>

#include <Core/Utils/Singleton.hpp>
#include <Core/Index/IndexMap.hpp>

namespace Ra { namespace Engine { class Entity; } }

namespace Ra { namespace Engine {

class EntityManager : public Core::Singleton<EntityManager>
{
    friend class Core::Singleton<EntityManager>;

public:
    /**
     * @brief Create an entity (kind of a factory).
     * Manager has the pointer ownership.
     * @return The created entity.
     */
    Entity* createEntity();

    Entity* createEntity(const std::string& name);

    /**
     * @brief Remove an entity given its index. Also deletes the pointer.
     * @param idx Index if the entity to remove.
     */
    void removeEntity(Core::Index idx);

    /**
     * @brief Remove a given entity. Also deletes the pointer.
     * @param entity The entity to remove.
     */
    void removeEntity(Entity* entity);

    /**
     * @brief Get an entity given its index.
     * @param idx Index of the component to retrieve.
     * @return The entity if found in the map, nullptr otherwise.
     */
    Entity* getEntity(Core::Index idx) const;

    /**
     * @brief Get all entities from the manager.
     * This might be usefull to be able to display and navigate through them
     * in a GUI for example.
     * @return A list containing all entities from the manager.
     */
    std::vector<Entity*> getEntities() const;

private:
    /// CONSTRUCTOR
    EntityManager() {}

    /// DESTRUCTOR
    virtual ~EntityManager();

private:
    Core::IndexMap<std::shared_ptr<Entity>> m_entities;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENTITYMANAGER_HPP
