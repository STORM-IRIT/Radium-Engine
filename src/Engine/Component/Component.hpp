#ifndef RADIUMENGINE_COMPONENT_HPP
#define RADIUMENGINE_COMPONENT_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Ray.hpp>

namespace Ra {
namespace Engine {
class System;
class Entity;
class RenderObject;
class RenderObjectManager;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * @brief A component is an element that can be updated by a system.
 * It is also linked to some other components in an entity.
 * Each component share a transform through their entity.
 */
class RA_ENGINE_API Component {
  public:
    Component( const std::string& name, Entity* entity );

    virtual ~Component();

    /**
     * @brief Pure virtual method to be overridden by any component.
     * When this method is called you are guaranteed that all other startup systems
     * have been loaded.
     */
    virtual void initialize() = 0;

    /**
     * @brief Set entity the component is part of.
     * This method is called by the entity.
     * @param entity The entity the component is part of.
     */
    virtual void setEntity( Entity* entity );

    /// Return the entity the component belongs to
    virtual Entity* getEntity() const;

    /// Return the component's name
    virtual const std::string& getName() const;

    /// Set the system to which the  component belongs.
    virtual void setSystem( System* system );

    /// Returns the system to which the component belongs.
    virtual System* getSystem() const;

    /// Add a new render object to the component. This adds the RO to the manager for drawing.
    virtual Core::Index addRenderObject( RenderObject* renderObject ) final;

    /// Remove the render object from the component.
    virtual void removeRenderObject( Core::Index roIdx ) final;

    /// Perform a ray cast query and print the result to the Info output.
    virtual void rayCastQuery( const Core::Ray& ray ) const;

    /// \name Editable transform interface.
    /// This allow to edit the data in the component with a render object
    /// as a key. An invalid RO index can be passed, meaning no specific RO is
    /// queried.
    ///@{

    /// Returns true if a transform can be edited with the render object index given as a key.
    virtual bool canEdit( Core::Index roIdx ) const { return false; }

    /// Get the transform associated with the given RO index key.
    virtual Core::Transform getTransform( Core::Index roIdx ) const {
        return Core::Transform::Identity();
    }

    /// Set the new transform associated with the RO index key.
    virtual void setTransform( Core::Index roIdx, const Core::Transform& transform ) {}

    ///@}

    /// Remove \p idx from the list of RenderObject indices.
    void notifyRenderObjectExpired( const Core::Index& idx );

  protected:
    /// Shortcut to access the render object manager.
    static RenderObjectManager* getRoMgr();

  public:
    /// The list of RenderObject indices.
    std::vector<Core::Index> m_renderObjects;

  protected:
    /// The name of the Component.
    std::string m_name;

    /// The Entity the Component belongs to.
    Entity* m_entity;

    /// The System managing the Component.
    System* m_system;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Component/Component.inl>

#endif // RADIUMENGINE_COMPONENT_HPP
