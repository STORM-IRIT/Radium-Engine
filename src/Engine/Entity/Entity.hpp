#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <map>
#include <mutex>
#include <thread>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Math/Vector.hpp>
#include <Core/Math/Matrix.hpp>

namespace Ra
{

class Component;

class Entity : public Core::IndexedObject
{
public:
    Entity();
    ~Entity();

    inline void setTransform(const Core::Transform& transform);
    inline void setTransform(const Core::Matrix4& transform);
    Core::Transform getTransform() const;
    Core::Matrix4 getTransformAsMatrix() const;

    void addComponent(Component* component);

    void removeComponent(Core::Index idx);
    void removeComponent(Component* component);

    Component* getComponent(Core::Index idx);

private:
    typedef std::pair<Core::Index, Component*> ComponentByIndex;
    std::map<Core::Index, Component*> m_components;

    Core::Transform m_transform;

    mutable std::mutex m_transformMutex;
};

} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
