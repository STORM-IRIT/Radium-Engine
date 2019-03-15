#ifndef RADIUMENGINE_ITEM_ENTRY_HPP_
#define RADIUMENGINE_ITEM_ENTRY_HPP_

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Index.hpp>

#include <string>
#include <vector>

namespace Ra {
namespace Engine {
class RadiumEngine;
class Entity;
class Component;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
/**
 * This represents a handle for an Entity, Component or RenderObject.
 * There are 4 possible states for this object:
 * - All members are invalid -> No object.
 * - Entity pointer valid, all others invalid -> Entity
 * - Entity and Component pointers valid, RenderObject index invalid -> Component
 * - Entity, Component and RenderObject index valid : -> RenderObject
 */
struct RA_ENGINE_API ItemEntry {
    /**
     * Create an invalid entry.
     */
    ItemEntry() = default;

    /**
     * Create an entry for the last non-null object:
     * - ItemEntry(entity) creates an Entity entry.
     * - ItemEntry(entity, component) creates a Component entry.
     * - ItemEntry(entity, component, ro) creates a RenderObject entity.
     */
    explicit ItemEntry( Ra::Engine::Entity* ent, Ra::Engine::Component* comp = nullptr,
                        Ra::Core::Utils::Index ro = Ra::Core::Utils::Index::Invalid() ) :
        m_entity{ent},
        m_component{comp},
        m_roIndex{ro} {}

    /**
     * Compare two items.
     */
    inline bool operator==( const ItemEntry& rhs ) const;

    /**
     * Returns true if the item represents any valid object.
     */
    bool isValid() const;

    /**
     * Returns true if the item can be selected.
     */
    bool isSelectable() const;

    /**
     * Returns true if the item represents an Entity.
     */
    inline bool isEntityNode() const;

    /**
     * Returns true if the item represents a Component.
     */
    inline bool isComponentNode() const;

    /**
     * Returns true if the item represents a RenderObject.
     */
    inline bool isRoNode() const;

    /**
     * Debug checks.
     */
    inline void checkConsistency() const;

    /**
     * The entity represented by the item, or owning the object represented.
     */
    Ra::Engine::Entity* m_entity{nullptr};

    /**
     * Component represented by the item or owning the represented RO.
     * If null, the item represents an entity.
     */
    Ra::Engine::Component* m_component{nullptr};

    /// RenderObject index of the represented object.
    Ra::Core::Utils::Index m_roIndex{};
};

/// \name ItemEntry data access
/// \{

/**
 * Returns the name associated to the given item.
 */
RA_ENGINE_API std::string getEntryName( const Engine::RadiumEngine* engine, const ItemEntry& ent );

/**
 * Returns a list of all the render object indices belonging to the given item:
 *  - RenderObject item : it returns only the RenderObject index.
 *  - Component item : it returns all its RenderObject indices.
 *  - Entity item : all RenderObject indices from all Components of the Entity.
 */
RA_ENGINE_API std::vector<Ra::Core::Utils::Index> getItemROs( const Engine::RadiumEngine* engine,
                                                              const ItemEntry& ent );
/// \}

} // namespace Engine
} // namespace Ra

#include <Engine/ItemModel/ItemEntry.inl>

#endif // RADIUMENGINE_ITEM_ENTRY_HPP_
