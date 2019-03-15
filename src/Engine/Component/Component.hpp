#ifndef RADIUMENGINE_COMPONENT_HPP
#define RADIUMENGINE_COMPONENT_HPP

#include <Core/Utils/Index.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Engine/RaEngine.hpp>

#include <vector>

#include <Core/Types.hpp>

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
 * \brief A component is an element that can be updated by a System.
 * It is also linked to some other Components in an Entity.
 * All Components share a transform through their Entity.
 */
class RA_ENGINE_API Component {
  public:
    Component( const std::string& name, Entity* entity );

    virtual ~Component();

    /**
     * \brief Pure virtual method to be overridden by any Component.
     * When this method is called you are guaranteed that all other startup
     * Systems have been loaded.
     */
    virtual void initialize() = 0;

    /**
     * \brief Set Entity the Component is part of.
     * This method is called by the Entity.
     * \param entity The entity the Component is part of.
     */
    virtual void setEntity( Entity* entity );

    /**
     * Return the entity the Component belongs to.
     */
    virtual Entity* getEntity() const;

    /**
     * Return the Component's name.
     */
    virtual const std::string& getName() const;

    /**
     * Set the System to which the Component belongs.
     */
    virtual void setSystem( System* system );

    /**
     * Returns the System to which the Component belongs.
     */
    virtual System* getSystem() const;

    /**
     * Add a new RenderObject to the Component.
     * \note This also adds the RenderObject to the manager for drawing.
     */
    Core::Utils::Index addRenderObject( RenderObject* renderObject );

    /**
     * Remove the RenderObject from the Component.
     * \note This also removes the RenderObject from the manager for drawing.
     */
    void removeRenderObject( const Core::Utils::Index& roIdx );

    /**
     * Perform a ray cast query and print the result to the logInfo output.
     */
    virtual void rayCastQuery( const Ra::Core::Ray& ray ) const;

    /** \name Editable transform interface
     * This allow to edit the data in the component with a render object
     * as a key. An invalid RO index can be passed, meaning no specific RO is
     * queried.
     */
    /// \{

    /**
     * Return true if a transform can be edited with the RenderObject index
     * given as a key, false otherwise.
     */
    virtual bool canEdit( const Core::Utils::Index& /*roIdx*/ ) const { return false; }

    /**
     * Return the transform associated with the given RenderObject index key.
     */
    virtual Core::Transform getTransform( const Core::Utils::Index& /*roIdx*/ ) const {
        return Core::Transform::Identity();
    }

    /**
     * Set the new transform associated with the RenderObject index key.
     */
    virtual void setTransform( const Core::Utils::Index& /*roIdx*/,
                               const Core::Transform& /*transform*/ ) {}
    /// \}

    /**
     * Remove \p idx from the list of RenderObject indices.
     */
    void notifyRenderObjectExpired( const Core::Utils::Index& idx );

  protected:
    /**
     * Shortcut to access the Engine's RenderObjectManager.
     */
    static RenderObjectManager* getRoMgr();

  public:
    /// The list of RenderObject indices.
    std::vector<Core::Utils::Index> m_renderObjects;

  protected:
    /// The name of the Component.
    std::string m_name{};

    /// The Entity the Component belongs to.
    Entity* m_entity{nullptr};

    /// The System managing the Component.
    System* m_system{nullptr};
};

} // namespace Engine
} // namespace Ra

#include <Engine/Component/Component.inl>

#endif // RADIUMENGINE_COMPONENT_HPP
