#ifndef RADIUMENGINE_ENTITYMANAGER_HPP
#define RADIUMENGINE_ENTITYMANAGER_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Core/Utils/IndexMap.hpp>
#include <Core/Utils/Singleton.hpp>

namespace Ra {
namespace Engine {
class Entity;
}
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * The EntityManager is responsible for storing the Engine's Entities.
 */
class RA_ENGINE_API EntityManager {
  public:
    EntityManager();

    /**
     * Copy operator is forbidden.
     */
    EntityManager( const EntityManager& ) = delete;

    /**
     * Assignment operator is forbidden.
     */
    EntityManager& operator=( const EntityManager& ) = delete;

    ~EntityManager() = default;

    /**
     * \brief Create an Entity with the given name.
     * \return The created Entity.
     * \note If \p name is an empty string, the Entity will be created with a
     *       generic name, e.g.\ "Entity_xx".
     * \note If an Entity with the given name already exists, the Entity will be
     *       created with name \p name_n, where n is the number of Entities
     *       with base name \p name.
     */
    Entity* createEntity( const std::string& name = "" );

    /**
     * \brief Check wether an Entity with a given name exists or not.
     * \param name The name of the Entity to find.
     * \return true if the Entity exists, false otherwise.
     */
    bool entityExists( const std::string& name ) const;

    /**
     * \brief Remove an Entity given its index.
     * \param idx Index if the Entity to remove.
     * \note \p idx must be a valid Entity Index.
     * \note \p idx is considered invalid after removal.
     */
    void removeEntity( Core::Utils::Index idx );

    /**
     * \brief Remove a given Entity.
     * \param entity The Entity to remove.
     * \note \p entity must be a registered Entity.
     */
    void removeEntity( Entity* entity );

    /**
     * \brief Get an Entity given its index.
     * \param idx Index of the Entity to find.
     * \return The Entity if it exists, nullptr otherwise.
     */
    Entity* getEntity( Core::Utils::Index idx ) const;

    /**
     * \brief Get an entity given its name.
     * \param name Name of the Entity to find.
     * \return The Entity if it exists, nullptr otherwise.
     */
    Entity* getEntity( const std::string& name ) const;

    /**
     * \brief Get all Entities from the manager.
     * This might be usefull to be able to display and navigate through them
     * in a gui for example.
     * \return A list containing all Entities from the manager.
     */
    std::vector<Entity*> getEntities() const;

    /**
     * \brief Update the Entities transformations.
     */
    void swapBuffers();

    /**
     * \brief Removes all entities.
     */
    void deleteEntities();

  private:
    /// The managed Entities.
    Core::Utils::IndexMap<std::unique_ptr<Entity>> m_entities;

    /// The map from Entity name to storage index.
    std::map<std::string, Core::Utils::Index> m_entitiesName;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENTITYMANAGER_HPP
