#ifndef RADIUMENGINE_COMPONENT_HPP
#define RADIUMENGINE_COMPONENT_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Math/Vector.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra { namespace Engine { class System; } }
namespace Ra { namespace Engine { class Entity; } }

namespace Ra { namespace Engine {

/**
 * @brief A component is an element that can be updated by a system.
 * It is also linked to some other components in an entity.
 * Each component share a transform through their entity.
 */
class Component : public Core::IndexedObject
{
public:
    /// CONSTRUCTOR
    Component() : IndexedObject() {}

    // FIXME (Charly) : Should destructor call something like
    //                  System::removeComponent(this) ?
    /// DESTRUCTOR
    virtual ~Component() {}

    /**
     * @brief Pure virtual method to be overrided by any component.
     * This update method is time agnostic (e.g. called by a render system).
     */
    virtual void update() = 0;

    /**
     * @brief Pure virtual method to be overrided by any component.
     * This update method depends on time (e.g. called by a physics system).
     * @param dt Time elapsed since last call.
     */
    virtual void update(Scalar dt) = 0;

    /**
     * @brief Set system the component is updated by.
     * This method is called by the system.
     * @param system The system the component is updated by.
     */
    virtual void setSystem(System* system);

    /**
     * @brief Set entity the component is part of.
     * This method is called by the entity.
     * @param entity The entity the component is part of.
     */
    virtual void setEntity(Entity* entity);

    virtual void setSelected(bool selected);

protected:
    System* m_system;
    Entity* m_entity;

    bool m_isSelected;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_COMPONENT_HPP
