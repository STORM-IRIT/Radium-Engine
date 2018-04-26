#ifndef RADIUMENGINE_SYSTEM_HPP
#define RADIUMENGINE_SYSTEM_HPP

#include <Engine/RaEngine.hpp>

#include <vector>
#include <memory>


namespace Ra {
namespace Core {
class TaskQueue;
} // namespace Core

namespace Engine {
struct FrameInfo;
class Component;
class Entity;
} // namespace Engine

namespace Asset {
class FileData;
}
} // namespace Ra

namespace Ra {
namespace Engine {

/// Systems are responsible of updating a specific subset of the components of each entity.
/// They can provide factory methods to create components, but their main role is to keep a
/// list of "active" components associated to an entity.
/// At each frame, each system loaded into the engine will be queried for tasks.
/// The goal of the tasks is to update the active components during the frame.
class RA_ENGINE_API System {
  public:
    System();
    virtual ~System();

    /**
     * @brief Pure virtual method to be overridden by any system.
     * A very basic version of this method could be to iterate on components
     * and just call Component::update() method on them.
     * This update depends on time (e.g. physics system).
     *
     * @param dt Time elapsed since last call.
     */
    virtual void generateTasks( Core::TaskQueue* taskQueue,
                                const Engine::FrameInfo& frameInfo ) = 0;

    /// Registers a component belonging to an entity, making it active within the system.
    void registerComponent( const Entity* entity, Component* component );

    /// Unregisters a component. The system will not update it.
    void unregisterComponent( const Entity* entity, Component* component );

    /// Removes all components belonging to a given entity.
    void unregisterAllComponents( const Entity* entity );

    /// Returns the components stored for the given entity.
    std::vector<Component*> getEntityComponents( const Entity* entity );

    /**
     * Factory method for component creation from file data.
     * Given a given file and the corresponding entity, the system will create the
     * corresponding components,add them to the entity.
     */
    virtual void handleAssetLoading( Entity* entity, const Asset::FileData* data ) {}

  protected:
    /// List of active components.
    std::vector<std::pair<const Entity*, Component*>> m_components;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SYSTEM_HPP
