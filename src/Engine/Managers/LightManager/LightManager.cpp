#include <Engine/Managers/LightManager/LightManager.hpp>


namespace Ra {
    namespace Engine {

        LightManager::LightManager()
        : m_data( nullptr )
        {
        }

        LightManager::~LightManager()
        {
        }

        void LightManager::generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo )
        {
        }
    
        void LightManager::handleAssetLoading( Entity* entity, const Asset::FileData* data )
        {
            // ...
        }
    }
}
