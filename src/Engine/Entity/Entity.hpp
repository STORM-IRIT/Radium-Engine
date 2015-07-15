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

class Entity : public IndexedObject
{
public:
    Entity();
    ~Entity();

    inline void setTransform(const Transform& transform);
    inline void setTransform(const Matrix4& transform);
    Transform getTransform() const;
    Matrix4 getTransformAsMatrix() const;

    void addComponent(Component* component);

    void removeComponent(Index idx);
    void removeComponent(Component* component);

    Component* getComponent(Index idx);

private:
    typedef std::pair<Index, Component*> ComponentByIndex;
    std::map<Index, Component*> m_components;

    Transform m_transform;

    mutable std::mutex m_transformMutex;
};

} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
