#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <Engine/RaEngine.hpp>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <Core/Index/IndexedObject.hpp>
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
class RA_ENGINE_API Entity : public Core::IndexedObject {
  public:
    RA_CORE_ALIGNED_NEW
    explicit Entity( const std::string& name = "" );

    // Entities are not copyable.
    Entity( const Entity& entity ) = delete;

    Entity& operator=( const Entity& ) = delete;

    virtual ~Entity();

    /// Return the name of the Entity.
    inline const std::string& getName() const;

    /// Set the name of the Entity.
    inline void rename( const std::string& name );

    /// Register the new transform for the Entity.
    /// \note The transform will be effectively set by calling swapTransformBuffers();
    inline void setTransform( const Core::Transform& transform );

    /// Register the new transform for the Entity.
    /// \note The transform will be effectively set by calling swapTransformBuffers();
    inline void setTransform( const Core::Matrix4& transform );

    /// Return the transform of the Entity.
    Core::Transform getTransform() const;

    /// Return the transform of the Entity.
    Core::Matrix4 getTransformAsMatrix() const;

    /// Actually sets the new transform for the Entity if required.
    void swapTransformBuffers();

    /// Add a component to the given entity.
    /// \note Component ownership is transfered to the entity.
    void addComponent( Component* component );

    /// Deletes a component with a given name.
    void removeComponent( const std::string& name );

    /// Get component with a given name.
    Component* getComponent( const std::string& name );

    /// Get component with a given name.
    const Component* getComponent( const std::string& name ) const;

    /// Get all the Components.
    const std::vector<std::unique_ptr<Component>>& getComponents() const;

    // Get component belonging to a given system.
    // Component* getComponent( const System& system);

    /// Get the number of Components.
    inline uint getNumComponents() const;

    /// Perform a ray cast query on each Component.
    virtual void rayCastQuery( const Core::Ray& r ) const;

  private:
    /// The transformation of the Entity.
    Core::Transform m_transform;

    /// The new transformation for the Entity.
    Core::Transform m_doubleBufferedTransform;

    /// The name of the Entity.
    std::string m_name;

    /// The Components of the Entity.
    std::vector<std::unique_ptr<Component>> m_components;

    /// Whether the transformation is up-to-date.
    bool m_transformChanged;

    /// Mutex to guard transformation manip against thread concurrency.
    mutable std::mutex m_transformMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
