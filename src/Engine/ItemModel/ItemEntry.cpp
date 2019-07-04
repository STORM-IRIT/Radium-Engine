
#include <Engine/ItemModel/ItemEntry.hpp>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra {
namespace Engine {

std::string getEntryName( const Engine::RadiumEngine* engine, const ItemEntry& ent ) {
    if ( ent.isValid() )
    {
        if ( ent.isRoNode() )
        { return engine->getRenderObjectManager()->getRenderObject( ent.m_roIndex )->getName(); }
        else if ( ent.isComponentNode() )
        { return ent.m_component->getName(); }
        else if ( ent.isEntityNode() )
        { return ent.m_entity->getName(); }
    }
    return "Invalid Entry";
}

std::vector<Ra::Core::Utils::Index> getItemROs( const Engine::RadiumEngine* /*engine*/,
                                                const ItemEntry& ent ) {
    std::vector<Ra::Core::Utils::Index> result;
    if ( ent.isValid() )
    {
        if ( ent.isRoNode() ) { result.push_back( ent.m_roIndex ); }
        else if ( ent.isComponentNode() )
        { result = ent.m_component->m_renderObjects; }
        else if ( ent.isEntityNode() )
        {
            for ( const auto& c : ent.m_entity->getComponents() )
            {
                result.insert( result.end(), c->m_renderObjects.begin(), c->m_renderObjects.end() );
            }
        }
    }
    return result;
}

bool ItemEntry::isValid() const {
    ON_DEBUG( checkConsistency() );
    Engine::RadiumEngine* engine = Engine::RadiumEngine::getInstance();
    return m_entity != nullptr           // It has an entity
           && engine->getEntityManager() // Is entityManager up ?
           && engine->getEntityManager()->entityExists( m_entity->getName() ) // The entity exists
           && ( ( !isRoNode() ||
                  engine->getRenderObjectManager()->exists( m_roIndex ) ) ); // The RO exists
}

bool ItemEntry::isSelectable() const {
    Engine::RadiumEngine* engine = Engine::RadiumEngine::getInstance();

    if ( m_entity->getIndex() == Engine::SystemEntity::getInstance()->getIndex() ) { return false; }

    if ( isRoNode() )
    {
        const bool isUI =
            engine->getRenderObjectManager()->getRenderObject( m_roIndex )->getType() ==
            Engine::RenderObjectType::UI;
        const bool isDebug =
            engine->getRenderObjectManager()->getRenderObject( m_roIndex )->getType() ==
            Engine::RenderObjectType::Debug;
        return ( !( isUI || isDebug ) );
    }

    return true;
}

} // namespace Engine
} // namespace Ra
