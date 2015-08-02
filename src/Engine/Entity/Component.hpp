#ifndef RADIUMENGINE_COMPONENT_HPP
#define RADIUMENGINE_COMPONENT_HPP

#include <Core/CoreMacros.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra { namespace Engine { class System;          } }
namespace Ra { namespace Engine { class Entity;          } }
namespace Ra { namespace Engine { class RenderObjectManager; } }

namespace Ra { namespace Engine {

/**
 * @brief A component is an element that can be updated by a system.
 * It is also linked to some other components in an entity.
 * Each component share a transform through their entity.
 */
class RA_API Component
{
public:
    /// CONSTRUCTOR
    Component(const std::string& name);

    // FIXME (Charly) : Should destructor call something like
    //                  System::removeComponent(this) ?
    /// DESTRUCTOR
    virtual ~Component();

	/**
	* @brief Pure virtual method to be overrided by any component.
	* When this method is called you are guaranteed to know your system and your entity
	*/
	virtual void initialize() = 0;
    /**
     * @brief Set entity the component is part of.
     * This method is called by the entity.
     * @param entity The entity the component is part of.
     */
    virtual void setEntity(const Entity* entity);
	
    virtual void setRenderObjectManager(RenderObjectManager* renderObjectManager);

    virtual const Entity* getEntity() const;

    virtual void setSelected(bool selected);

    virtual const std::string& getName() const;

protected:
    std::string m_name;

    const Entity* m_entity;

    RenderObjectManager* m_renderObjectManager;

    bool m_isSelected;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Entity/Component.inl>

#endif // RADIUMENGINE_COMPONENT_HPP
