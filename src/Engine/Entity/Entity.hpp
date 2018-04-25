#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <Engine/RaEngine.hpp>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <Core/Container/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Ray.hpp>

namespace Ra {
namespace Engine {
class Component;
class System;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/// An entity is an scene element. It ties together components with a transform.
class RA_ENGINE_API Entity : public Core::Container::IndexedObject {
  public:
    RA_CORE_ALIGNED_NEW
    explicit Entity( const std::string& name = "" );

    // Entities are not copyable.
    Entity( const Entity& entity ) = delete;
    Entity& operator=( const Entity& ) = delete;

    virtual ~Entity();

    // Name
    inline const std::string& getName() const;
    inline void rename( const std::string& name );

    // Transform
    inline void setTransform( const Core::Math::Transform& transform );
    inline void setTransform( const Core::Math::Matrix4& transform );
    Core::Math::Transform getTransform() const;
    Core::Math::Matrix4 getTransformAsMatrix() const;

    void swapTransformBuffers();

    // Components
    /// Add a component to the given entity. Component ownership is transfered to the entity.
    void addComponent( Component* component );

    /// Deletes a component with a given name.
    void removeComponent( const std::string& name );

    /// Get component with a given name.
    Component* getComponent( const std::string& name );
    const Component* getComponent( const std::string& name ) const;

    const std::vector<std::unique_ptr<Component>>& getComponents() const;

    /// Get component belonging to a given system.
    // Component* getComponent( const System& system);

    inline uint getNumComponents() const;

    // Queries
    virtual void rayCastQuery( const Core::Math::Ray& r ) const;

  private:
    Core::Math::Transform m_transform;
    Core::Math::Transform m_doubleBufferedTransform;

    std::string m_name;

    std::vector<std::unique_ptr<Component>> m_components;

    bool m_transformChanged;
    mutable std::mutex m_transformMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
