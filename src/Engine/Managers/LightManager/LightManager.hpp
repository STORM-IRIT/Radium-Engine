#ifndef RADIUMENGINE_LIGHTMANAGER_H
#define RADIUMENGINE_LIGHTMANAGER_H

#include <Engine/RaEngine.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Renderer/Light/LightStorage.hpp>

#include <memory>


namespace Ra {
    namespace Engine {
    }
}


namespace Ra {
    namespace Engine {

        /**
         * Interface providing functions to manage a group or type of lights
         * in a specific way.
         */
        class RA_ENGINE_API LightManager : public System
        {
        public:
            /// Constructor
            LightManager();

            // Make copies impossible
            LightManager( const LightManager& ) = delete;
            LightManager& operator=( const LightManager& ) = delete;

            /// Virtual destructor
            virtual ~LightManager();

            // Calls for the Renderer

            /**
             * @brief Call before a render, update the general state of the LightManager.
             */
            virtual void preprocess() = 0;

            /**
             * @brief Call before a render, process what is needed for a given Light.
             */
            virtual void prerender(Light& li) = 0;

            /**
             * @brief Call after a render, process what is needed for a given Light.
             */
            virtual void postrender(Light& li) = 0;

            /**
             * @brief Call after a render, update the general state of the LightManager.
             */
            virtual void postprocess() = 0;

            // System methods

            /// Method generating the correct tasks for a LightManager.
            void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

            /// Handle Lights loading.
            void handleAssetLoading( Entity* entity, const Asset::FileData* data ) override;

        private:
            /// Stores the object that stores the lights...
            std::unique_ptr<LightStorage> m_data;
        };

    }
}

#endif //RADIUMENGINE_LIGHTMANAGER_H
