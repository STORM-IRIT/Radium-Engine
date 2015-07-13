#ifndef RADIUMENGINE_COMPONENTMANAGER_HPP
#define RADIUMENGINE_COMPONENTMANAGER_HPP

#include <memory>

#include <Core/Utils/Singleton.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>

namespace Ra
{

class Component;

class ComponentManager : public Singleton<ComponentManager>
{
    friend class Singleton<ComponentManager>;

public:
    /**
     * @brief Add a component to the manager.
     * Manager takes ownership of the component. It also sets its index.
     * @param component The component to add to the manager.
     */
    void addComponent(Component* component);

    /**
     * @brief Remove a component given its index. Also deletes the pointer.
     * @param idx Index of the component to remove.
     */
    void removeComponent(Index idx);

    /**
     * @brief Remove a given component. Also deletes the pointer.
     * @param component The component to remove.
     */
    void removeComponent(Component* component);

    /**
     * @brief Get a component given its index
     * @param idx Index of the component to retrieve.
     * @return The component if found in the map, nullptr otherwise.
     */
    Component* getComponent(Index idx) const;

private:
    /// CONSTRUCTOR
    ComponentManager() {}

    /// DESTRUCTOR
    virtual ~ComponentManager();

private:
    IndexMap<std::shared_ptr<Component>> m_components;
};

} // namespace Ra

#endif // RADIUMENGINE_COMPONENTMANAGER_HPP
