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
    void removeComponent(Index idx);
    void removeComponent(Component* component);

    Component* getComponent(Index idx) const;

private:
    ComponentManager() {}
    virtual ~ComponentManager();

private:
    IndexMap<std::shared_ptr<Component>> m_components;
};

} // namespace Ra

#endif // RADIUMENGINE_COMPONENTMANAGER_HPP
