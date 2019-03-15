#ifndef RADIUMENGINE_SYSTEM_HPP
#define RADIUMENGINE_SYSTEM_HPP

#include <Engine/RaEngine.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Core {
class TaskQueue;
namespace Asset {
class FileData;
}
} // namespace Core

namespace Engine {
struct FrameInfo;
class Component;
class Entity;
} // namespace Engine

namespace Engine {

/**
 * Systems are responsible for updating a specific subset of the Components of each Entity.
 * They can provide factory methods to create Components, but their main role is to keep a
 * list of "active" Components associated to an Entity.
 * At each frame, each System loaded into the RadiumEngine will be queried for Tasks.
 * The goal of the Tasks is to update the active Components during the frame.
 */
class RA_ENGINE_API System {
    friend class Component;

  public:
    System() = default;

    virtual ~System() = default;

    /**
     * Factory method for Component creation from a FileData.
     * From a given FileData and the corresponding Entity, the System will create the
     * corresponding Components, add them to the Entity, and register them.
     */
    virtual void handleAssetLoading( Entity* entity, const Core::Asset::FileData* data ) {}

    /**
     * \brief Pure virtual method to be overridden by any System.
     * Must register in \p taskQueue the operations that must be done at each frame.
     *
     * \param taskQueue The queue to fill.
     * \param frameInfo Information about the current frame (see FrameInfo).
     */
    virtual void generateTasks( Core::TaskQueue* taskQueue,
                                const Engine::FrameInfo& frameInfo ) = 0;

    /**
     * Return the list of Components managed for the given Entity.
     */
    std::vector<Component*> getEntityComponents( const Entity* entity );

  protected:
    /**
     * Register a component belonging to an Entity, making it active within the System.
     * \note If a System overrides this method, it must explicitely call it
     *       before doing any specific stuff.
     */
    // FIXME: should get the entity through the Component!
    virtual void registerComponent( const Entity* entity, Component* component );

    /**
     * Unregisters the given Component. The System will not update it anymore.
     * \note If a System overrides this method, it must explicitely call it
     *       before doing any specific stuff.
     */
    // FIXME: should get the entity through the Component!
    virtual void unregisterComponent( const Entity* entity, Component* component );

    /**
     * Removes all Components belonging to the given Entity.
     * \note If a System overrides this function, it must call the inherited method.
     */
    virtual void unregisterAllComponents( const Entity* entity );

  protected:
    /// List of active Components.
    std::vector<std::pair<const Entity*, Component*>> m_components;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SYSTEM_HPP
