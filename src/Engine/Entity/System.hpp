#ifndef RADIUMENGINE_SYSTEM_HPP
#define RADIUMENGINE_SYSTEM_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <memory>

#include <Core/Utils/Any.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

#include <Engine/Entity/Component.hpp>

namespace Ra
{
    namespace Core
    {
        class TaskQueue;
    }
}

namespace Ra
{
    namespace Engine
    {
        struct FrameInfo;
        class Component;
        class Entity;
    }

    namespace Asset
    {
        class FileData;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API System
        {
        public:
            System();
            virtual ~System();

            /**
             * @brief Initialize system.
             */
            virtual void initialize() = 0;

            /**
             * @brief Pure virtual method to be overrided by any system.
             * A very basic version of this method could be to iterate on components
             * and just call Component::udate() method on them.
             * This update depends on time (e.g. physics system).
             *
             * @param dt Time elapsed since last call.
             */
            virtual void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) = 0;

            /**
             * @brief Add a component to the system.
             *
             * @param component The component to be added to the system
             * @param id The component id
             */
            void addComponent( Component* component );

            /**
             * @brief Remove a component from the system given its index.
             *
             * @param id The id of the component to remove
             */
            void removeComponent( const std::string& name );

            /**
              * @brief Remove a component from the system.
              * @param component The component to remove
              */
            void removeComponent( Component* component );

            /**
             * @brief Handle a keyboard event.
             * @param event The keyboard event to handle
             * @return true if the event has been handled, false otherwise.
             */
            virtual bool handleKeyEvent( const Core::KeyEvent& event )
            {
                return false;
            }

            /**
             * @brief Handle a mouse event.
             * @param event The mouse event to handle
             * @return true if the event has been handled, false otherwise.
             */
            virtual bool handleMouseEvent( const Core::MouseEvent& event )
            {
                return false;
            }

			virtual void handleFileLoading(Entity* entity, const std::string& filename) {}
			
            virtual void handleDataLoading( Entity* entity, const std::string& rootFolder,
                                            const std::map<std::string, Core::Any>& data ) {}

            virtual void handleAssetLoading( Entity* entity, const Asset::FileData* data ) {}

            /**
             * @brief Handle all the logic behind a component creation.
             * @param name Name of the entity the component should belong to.
             * @return The created component.
             */
            virtual Component* addComponentToEntity( Entity* entity ) final;
            
            // Register the system so that its callbackOnComponentCreation function will be called upon creation of a component
            void registerOnComponentCreation(Ra::Engine::System* system);

        protected:
            virtual Component* addComponentToEntityInternal( Entity* entity, uint id ) = 0;

            // This function must be called after a component has be fully created.
            void callOnComponentCreationDependencies(const Ra::Engine::Component* component);

            virtual void callbackOnComponentCreation(const Ra::Engine::Component* component) {}

        protected:
            std::map<std::string, std::shared_ptr<Component>> m_components;
            std::vector<System*> m_dependentSystems;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_SYSTEM_HPP
