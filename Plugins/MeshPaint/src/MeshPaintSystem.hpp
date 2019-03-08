#ifndef MESHPAINTPLUGIN_MESHPAINTSYSTEM_HPP
#define MESHPAINTPLUGIN_MESHPAINTSYSTEM_HPP

#include <MeshPaintPluginMacros.hpp>

#include <Core/Utils/Color.hpp>

#include <Engine/Renderer/Renderer.hpp>
#include <Engine/System/System.hpp>

namespace Ra {
namespace Engine {
class Entity;
class RenderTechnique;
class Component;
} // namespace Engine
} // namespace Ra

namespace MeshPaintPlugin {
class MeshPaintComponent;

/**
 * The MeshPaintSystem class is responsible for dispatching paint events to the
 * MeshPaintComponents.
 */
class MESH_PAINT_PLUGIN_API MeshPaintSystem : public Ra::Engine::System {
  public:
    MeshPaintSystem();

    ~MeshPaintSystem() override;

    void handleAssetLoading( Ra::Engine::Entity* entity,
                             const Ra::Core::Asset::FileData* fileData ) override;

    void generateTasks( Ra::Core::TaskQueue* taskQueue,
                        const Ra::Engine::FrameInfo& frameInfo ) override;

    /**
     * Prepare the MeshPaintComponents for painting.
     */
    void startPaintMesh( bool start );

    /**
     * Paint on the picked mesh.
     */
    void paintMesh( const Ra::Engine::Renderer::PickingResult& picking,
                    const Ra::Core::Utils::Color& color );
};

} // namespace MeshPaintPlugin

#endif // MESHPAINTPLUGIN_MESHPAINTSYSTEM_HPP
