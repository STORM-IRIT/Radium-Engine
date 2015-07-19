#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <string>
#include <map>
#include <mutex>
#include <thread>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Math/Vector.hpp>
#include <Core/Math/Matrix.hpp>

namespace Ra { namespace Engine { class Component; } }

namespace Ra { namespace Engine {

class Entity : public Core::IndexedObject
{
public:
    explicit Entity(const std::string& name = "");
    ~Entity();

    inline const std::string& getName() const;
    inline void rename(const std::string& name);

    inline void setTransform(const Core::Transform& transform);
    inline void setTransform(const Core::Matrix4& transform);
    Core::Transform getTransform() const;
    Core::Matrix4 getTransformAsMatrix() const;

    void setSelected(bool selected);

    void addComponent(Component* component);

    void removeComponent(Core::Index idx);
    void removeComponent(Component* component);

    Component* getComponent(Core::Index idx);
    std::vector<Component*> getComponents() const;

private:
    std::string m_name;

    typedef std::pair<Core::Index, Engine::Component*> ComponentByIndex;
    std::map<Core::Index, Engine::Component*> m_components;

    Core::Transform m_transform;

    mutable std::mutex m_mutex;
    bool m_isSelected;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
