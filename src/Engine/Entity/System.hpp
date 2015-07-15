#ifndef RADIUMENGINE_SYSTEM_HPP
#define RADIUMENGINE_SYSTEM_HPP

#include <map>

#include <Core/CoreMacros.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

namespace Ra
{

class Component;

class System
{
public:
    System();
    virtual ~System();

    /**
     * @brief Initialize system.
     */
    virtual void initialize() = 0;

    /**
     * @brief Pure virtual method to be overrided by any system.
     * A very basic version of this method could be to iterate on components
     * and just call Component::udate() method on them.
     * This update method is time agnostic (e.g. render system).
     */
    virtual void update() = 0;

    /**
     * @brief Pure virtual method to be overrided by any system.
     * A very basic version of this method could be to iterate on components
     * and just call Component::udate() method on them.
     * This update depends on time (e.g. physics system).
     *
     * @param dt Time elapsed since last call.
     */
    virtual void update(Scalar dt) = 0;

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
    void removeComponent(Index idx);

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
    virtual bool handleKeyEvent(const KeyEvent& event) { return false; }

    /**
     * @brief Handle a mouse event.
     * @param event The mouse event to handle
     * @return true if the event has been handled, false otherwise.
     */
    virtual bool handleMouseEvent(const MouseEvent& event) { return false; }

protected:
    std::map<Index, Component*> m_components;
};

} // namespace Ra

#endif // RADIUMENGINE_SYSTEM_HPP
