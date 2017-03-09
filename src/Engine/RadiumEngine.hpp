#ifndef RADIUMENGINE_ENGINE_HPP
#define RADIUMENGINE_ENGINE_HPP

#include <Engine/RaEngine.hpp>

#include <mutex>
#include <map>
#include <string>
#include <memory>
#include <vector>

#include <Core/Log/Log.hpp>
#include <Core/Utils/Singleton.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>

namespace Ra
{
    namespace Core
    {
        class TaskQueue;
        struct MouseEvent;
        struct KeyEvent;
    }
}
namespace Ra
{
    namespace Engine
    {
        class System;
        class Entity;
        class Component;
    }
}

namespace Ra
{
    namespace Engine
    {
        class RA_ENGINE_API RadiumEngine
        {
            RA_SINGLETON_INTERFACE(RadiumEngine);
        public:
            RadiumEngine();
            ~RadiumEngine();

            void initialize();
            void cleanup();

            void getTasks( Core::TaskQueue* taskQueue, Scalar dt );

            void registerSystem( const std::string& name,
                                 System* system );
            System* getSystem( const std::string& system ) const;

            bool loadFile( const std::string& file );

            /// Is called at the end of the frame to synchronize any data
            /// that may have been updated during the frame's multithreaded processing.
            void endFrameSync();

            /// Manager getters
            RenderObjectManager*  getRenderObjectManager()  const;
            EntityManager*        getEntityManager()        const;
            SignalManager*        getSignalManager()        const;

        private:
            std::map<std::string, std::shared_ptr<System>> m_systems;

            std::unique_ptr<RenderObjectManager> m_renderObjectManager;
            std::unique_ptr<EntityManager>       m_entityManager;
            std::unique_ptr<SignalManager>       m_signalManager;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_ENGINE_HPP
