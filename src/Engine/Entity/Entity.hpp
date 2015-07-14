#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <map>

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

    void setTransform(const Transform& transform) { m_transform = transform; }
    void setTransform(const Matrix4& transform) { m_transform = Transform(transform); }
    const Transform& getTransform() const { return m_transform; }
    Transform& getTransform() { return m_transform; }
    Matrix4 getTransformAsMatrix() const { return m_transform.matrix(); }

    void addComponent(Component* component);

    void removeComponent(Index idx);
    void removeComponent(Component* component);

    Component* getComponent(Index idx);

private:
    typedef std::pair<Index, Component*> ComponentByIndex;
    std::map<Index, Component*> m_components;

    Transform m_transform;
};

} // namespace Ra

#endif // RADIUMENGINE_ENTITY_HPP
