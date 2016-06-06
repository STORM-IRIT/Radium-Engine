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