#pragma once

#include <Engine/RaEngine.hpp>

#include <Core/Utils/Index.hpp>

#include <string>
#include <vector>

namespace Ra {
namespace Engine {
class RadiumEngine;

namespace Scene {

class Entity;
class Component;

/// This represent a handle for an entity, component or render object.
/// There are 4 possible states for this object :
/// * All members are invalid -> No object.
/// * Entity pointer valid, all others invalid -> Entity
/// * Entity and component pointers valid, RO index invalid -> Component
/// * Entity, component and RO index valid : -> RO
struct RA_ENGINE_API ItemEntry {
    /// Create an invalid entry.
    ItemEntry() = default;

    /// Create an entry.
    /// ItemEntry(entity) creates an entity entry.
    /// ItemEntry(entity, component) creates a component entry
    /// ItemEntry(entity, component, RO) creates a render object entity.
    explicit ItemEntry( Entity* ent,
                        Component* comp           = nullptr,
                        Ra::Core::Utils::Index ro = Ra::Core::Utils::Index::Invalid() ) :
        m_entity { ent }, m_component { comp }, m_roIndex { ro } {}

    /// Compare two items.
    inline bool operator==( const ItemEntry& rhs ) const;

    /// Returns true if the item represents any valid object.
    bool isValid() const;

    /// Returns true if the item can be selected.
    bool isSelectable() const;

    /// Returns true if the item represents an entity.
    inline bool isEntityNode() const;

    /// Returns true if the item represents a component.
    inline bool isComponentNode() const;

    /// Returns true if the item represents a render object.
    inline bool isRoNode() const;

    /// Debug checks.
    inline void checkConsistency() const;

    /// The entity represented by the item, or owning the object represented.
    Entity* m_entity { nullptr };

    /// Component represented by the item or owning the represented RO.
    /// If null, the item represents an entity.
    Component* m_component { nullptr };

    /// RO index of the represented object.
    Ra::Core::Utils::Index m_roIndex {};
};

/// Returns the name associated to the given item.
RA_ENGINE_API std::string getEntryName( const Engine::RadiumEngine* engine, const ItemEntry& ent );

/// Returns a list of all the render object indices belonging to the given item.
/// RO item : it returns only the RO index.
/// Component item : it returns all its ROs.
/// Entity item : all ROs from all compoents of given entity.
RA_ENGINE_API std::vector<Ra::Core::Utils::Index> getItemROs( const Engine::RadiumEngine* engine,
                                                              const ItemEntry& ent );
} // namespace Scene
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {
namespace Scene {

bool ItemEntry::isEntityNode() const {
    ON_DEBUG( checkConsistency() );
    return ( m_entity && !m_component );
}

bool ItemEntry::isComponentNode() const {
    ON_DEBUG( checkConsistency() );
    return ( m_entity && m_component && m_roIndex.isInvalid() );
}

bool ItemEntry::isRoNode() const {
    ON_DEBUG( checkConsistency() );
    return ( m_entity && m_component && m_roIndex.isValid() );
}

void ItemEntry::checkConsistency() const {
    CORE_ASSERT( m_entity || ( !m_component && m_roIndex.isInvalid() ),
                 "Component or RO  is set while entity is not" );
    CORE_ASSERT( m_component || m_roIndex.isInvalid(), "RO  is set while component is not" );
}

inline bool ItemEntry::operator==( const ItemEntry& rhs ) const {
    return m_entity == rhs.m_entity && m_component == rhs.m_component && m_roIndex == rhs.m_roIndex;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
