#ifndef RADIUMENGINE_SYSTEM_HPP
#define RADIUMENGINE_SYSTEM_HPP

#include <map>
#include <memory>

#include <Core/CoreMacros.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

namespace Ra { namespace Core   { class TaskQueue;  } }
namespace Ra { namespace Engine { class RadiumEngine; } }
namespace Ra { namespace Engine { struct FrameInfo; } }
namespace Ra { namespace Engine { class Component; } }
namespace Ra { namespace Engine { class Entity; } }

namespace Ra { namespace Engine {

class RA_API System
{
public:
    System(RadiumEngine* engine);
    virtual ~System();

    /**
     * @brief Initialize system.
     */
    virtual void initialize() = 0;

    /**
     * @brief Pure virtual method to be overrided by any system.
     * A very basic version of this method could be to iterate on components
     * and just call Component::udate() method on them.
     * This update depends on time (e.g. physics system).
     *
     * @param dt Time elapsed since last call.
     */
    virtual void generateTasks(Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo) = 0;

    /**
     * @brief Add a component to the system.
     *
     * @param component The component to be added to the system
     * @param id The component id
     */
    void addComponent(Component* component);

    /**
     * @brief Remove a component from the system given its index.
     *
     * @param id The id of the component to remove
     */
    void removeComponent(const std::string& name);

    /**
      * @brief Remove a component from the system.
      * @param component The component to remove
      */
    void removeComponent(Component* component);

    /**
     * @brief Handle a keyboard event.
     * @param event The keyboard event to handle
     * @return true if the event has been handled, false otherwise.
     */
    virtual bool handleKeyEvent(const Core::KeyEvent& event) { return false; }

    /**
     * @brief Handle a mouse event.
     * @param event The mouse event to handle
     * @return true if the event has been handled, false otherwise.
     */
    virtual bool handleMouseEvent(const Core::MouseEvent& event) { return false; }

	/**
	 * @brief Handle a file loading event. Does nothing by default.
	 * @param file The file to load
	 */
	virtual void handleFileLoading(const std::string& filename) { }

    /**
     * @brief Handle all the logic behind a component creation.
     * @param name Name of the entity the component should belong to.
     * @return The created component.
     */
    virtual Component* addComponentToEntity( Engine::Entity* entity ) = 0;

protected:
	RadiumEngine* m_engine;

    std::map<std::string, std::shared_ptr<Component>> m_components;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SYSTEM_HPP
