#ifndef RADIUMENGINE_SYSTEM_HPP
#define RADIUMENGINE_SYSTEM_HPP

#include <map>

#include <Core/CoreMacros.hpp>

namespace Ra
{

class Component;

class System
{
public:
    System();
    virtual ~System();
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
    void addComponent(Component* component, uint id);

    /**
     * @brief Remove a component from the system.
     *
     * @param id The id of the component to remove
     */
    void removeComponent(uint id);

private:
    std::map<uint, Component*> m_components;
};

} // namespace Ra

#endif // RADIUMENGINE_SYSTEM_HPP
