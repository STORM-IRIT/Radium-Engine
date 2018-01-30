#ifndef MESHPAINTPLUGIN_MESHPAINTSYSTEM_HPP
#define MESHPAINTPLUGIN_MESHPAINTSYSTEM_HPP

#include <MeshPaintPluginMacros.hpp>

#include <Engine/System/System.hpp>
#include <Engine/Renderer/Renderer.hpp>

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

namespace MeshPaintPlugin
{
    class MeshPaintComponent;
}

namespace MeshPaintPlugin
{
    class MESH_PAINT_PLUGIN_API MeshPaintSystem : public Ra::Engine::System
    {
    public:
        MeshPaintSystem();
        virtual ~MeshPaintSystem();

        virtual void handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData ) override;

        virtual void generateTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo ) override;

        void startPaintMesh( bool start );

        void paintMesh( const Ra::Engine::Renderer::PickingResult& picking, const Ra::Core::Color& color );
    };

} // namespace MeshPaintPlugin

#endif // MESHPAINTPLUGIN_MESHPAINTSYSTEM_HPP
