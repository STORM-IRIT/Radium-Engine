#ifndef FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP
#define FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP

#include <Engine/Entity/System.hpp>


namespace Ra
{
    namespace Core
    {
        struct TriangleMesh;
    }
}

namespace Ra
{
    namespace Engine
    {
        class RadiumEngine;
        class Entity;
        struct RenderTechnique;
        class Component;
        class FancyMeshComponent;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_API FancyMeshSystem : public System
        {
        public:
            FancyMeshSystem ( RadiumEngine* engine );
            virtual ~FancyMeshSystem();

            virtual void initialize() override;
            virtual void handleFileLoading ( const std::string& filename ) override;

            virtual Component* addComponentToEntity ( Engine::Entity* entity ) override;

            virtual void generateTasks ( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

            // Specialized factory methods for this systems.
            FancyMeshComponent* addDisplayMeshToEntity ( Entity* entity, const Core::TriangleMesh& mesh );
            FancyMeshComponent* addDisplayMeshToEntity ( Entity* entity,
                                                         const Core::TriangleMesh& mesh,
                                                         RenderTechnique* technique );
        };

    } // namespace Engine
} // namespace Ra

#endif // FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP
