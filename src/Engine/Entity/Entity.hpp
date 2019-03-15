#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <Engine/RaEngine.hpp>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <Core/Math/Types.hpp>
#include <Core/Utils/IndexedObject.hpp>

namespace Ra {
namespace Engine {
class Component;
class System;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * An entity is an scene element. It ties together Components with a transform.
 */
class RA_ENGINE_API Entity : public Core::Utils::IndexedObject {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    explicit Entity( const std::string& name = "" );

    /**
     * Copy operator is forbidden.
     */
    Entity( const Entity& entity ) = delete;

    /**
     * Assignment operator is forbidden.
     */
    Entity& operator=( const Entity& ) = delete;

    ~Entity() override;

    /**
     * Return the name of the Entity.
     */
    inline const std::string& getName() const;

    /**
     * Set the name of the Entity.
     */
    inline void rename( const std::string& name );

    /// \name Transform
    /// \{

    /**
     * Register the new transform for the Entity.
     * \note The transform will be effectively set by calling swapTransformBuffers();
     */
    inline void setTransform( const Core::Transform& transform );

    /**
     * Register the new transform for the Entity.
     * \note The transform will be effectively set by calling swapTransformBuffers();
     */
    inline void setTransform( const Core::Matrix4& transform );

    /**
     * Return the transform of the Entity.
     */
    const Core::Transform& getTransform() const;

    /**
     * Return the transform of the Entity.
     */
    const Core::Matrix4& getTransformAsMatrix() const;

    /**
     * Actually sets the new transform for the Entity if required.
     */
    void swapTransformBuffers();
    /// \}

    /// \name Components
    /// \{

    /**
     * Add a Component. Component ownership is transfered to the Entity.
     */
    void addComponent( Component* component );

    /**
     * Deletes the first Component found with the given name.
     */
    void removeComponent( const std::string& name );

    /**
     * Return the first Component found with the given name.
     */
    Component* getComponent( const std::string& name );

    /**
     * Return the first Component found with the given name.
     */
    const Component* getComponent( const std::string& name ) const;

    /**
     * Return the list of all the Components.
     */
    const std::vector<std::unique_ptr<Component>>& getComponents() const;

    /**
     * Return the number of Components.
     */
    inline uint getNumComponents() const;
    /// \}

    /**
     * Perform a ray cast query on each Component.
     */
    virtual void rayCastQuery( const Eigen::ParametrizedLine<Scalar, 3>& r ) const;

  private:
    /// The name of the Entity.
    std::string m_name{};

    /// The transformation of the Entity.
    Core::Transform m_transform;

    /// The Components of the Entity.
    std::vector<std::unique_ptr<Component>> m_components;

    /// The new transformation for the Entity.
    Core::Transform m_doubleBufferedTransform;

    /// Whether the transformation is up-to-date.
    bool m_transformChanged{false};

    /// Mutex to guard transformation manip against thread concurrency.
    mutable std::mutex m_transformMutex;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
