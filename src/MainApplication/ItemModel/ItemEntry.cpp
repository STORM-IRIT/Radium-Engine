#include <MainApplication/ItemModel/ItemEntry.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>


namespace Ra
{
    namespace Gui
    {

        std::string getEntryName( const Engine::RadiumEngine* engine, const ItemEntry& ent )
        {
            if ( ent.isValid() )
            {
                if ( ent.isRoNode() )
                {
                    return engine->getRenderObjectManager()->getRenderObject( ent.m_roIndex )->getName();
                }
                else if ( ent.isComponentNode() )
                {
                    return ent.m_component->getName();
                }
                else if ( ent.isEntityNode() )
                {
                    return ent.m_entity->getName();
                }
            }
            return "Invalid Entry";
        }

        std::vector<Ra::Core::Index> getItemROs(const Engine::RadiumEngine* engine, const ItemEntry& ent)
        {
            std::vector< Ra::Core::Index> result;
            if (ent.isValid())
            {
                if ( ent.isRoNode() )
                {
                    result.push_back(ent.m_roIndex);
                }
                else if ( ent.isComponentNode() )
                {
                    result = ent.m_component->m_renderObjects;
                }
                else if ( ent.isEntityNode() )
                {
                    for ( const auto& c : ent.m_entity->getComponents())
                    {
                        result.insert(result.end(), c->m_renderObjects.begin(), c->m_renderObjects.end());
                    }
                }
            }
            return result;
        }


        bool ItemEntry::isSelectable() const
        {
            Engine::RadiumEngine* engine = Engine::RadiumEngine::getInstance();
            if ( isRoNode() )
            {
                const bool isUI = engine->getRenderObjectManager()->getRenderObject( m_roIndex )->getType() ==
                    Engine::RenderObjectType::UI;
                const bool isDebug = engine->getRenderObjectManager()->getRenderObject( m_roIndex )->getType() ==
                    Engine::RenderObjectType::Debug;
                return (!(isUI || isDebug));
            }
            else
            {
                return true;
            }
        }

    }
}
