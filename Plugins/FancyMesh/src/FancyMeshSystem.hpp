#ifndef FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP
#define FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP

#include "FancyMeshPlugin.hpp"

#include <Engine/System/System.hpp>

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
        class Entity;
        struct RenderTechnique;
        class Component;
    }
}

namespace FancyMeshPlugin
{
    class FancyMeshComponent;
}

namespace FancyMeshPlugin
{
    class FM_PLUGIN_API FancyMeshSystem : public Ra::Engine::System
    {
    public:
        FancyMeshSystem();
        virtual ~FancyMeshSystem();

        virtual void handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData ) override;

        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

        // Specialized factory method for this systems.
        static FancyMeshComponent* makeFancyMeshFromGeometry( const Ra::Core::TriangleMesh& mesh, const std::string& name,
                                                              Ra::Engine::RenderTechnique* technique = nullptr );

    };

} // namespace FancyMeshPlugin

#endif // FANCYMESHPLUGIN_FANCYMESHSYSTEM_HPP
