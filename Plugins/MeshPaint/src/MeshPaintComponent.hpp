#ifndef MESHPAINTPLUGIN_MESHPAINTCOMPONENT_HPP
#define MESHPAINTPLUGIN_MESHPAINTCOMPONENT_HPP

#include <MeshPaintPluginMacros.hpp>

#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Utils/Color.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Renderer.hpp>

namespace Ra {
namespace Core {
class TaskQueue;
struct TaskParams;
} // namespace Core
namespace Engine {
struct FrameInfo;
}
} // namespace Ra

namespace MeshPaintPlugin {

/**
 * The MeshPaintComponent is responsible for storing the colors painted on an object.
 * For now does not handle textured meshes.
 */
class MESH_PAINT_PLUGIN_API MeshPaintComponent : public Ra::Engine::Component {
  public:
    MeshPaintComponent( const std::string& name, Ra::Engine::Entity* entity );

    ~MeshPaintComponent() override;

    void initialize() override;

    /**
     * Register in taskQueue the operations that must be done at each frame.
     */
    void addTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& info );

    /**
     * Registers the Entity name for Component communication.
     */
    void setDataId( const std::string& id );

    /**
     * Toggle on/off painting on the mesh.
     */
    void startPaint( bool on );

    /**
     * Paints the color on the selected vertices.
     */
    void paintMesh( const Ra::Engine::Renderer::PickingResult& picking,
                    const Ra::Core::Utils::Color& color );

  private:
    /// Getter for the RenderObject index.
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Utils::Index>::Getter
        m_renderObjectReader;

    /// Entity name for Component communication.
    std::string m_dataId;

    /// Initial RO shader config when not painting.
    Ra::Engine::ShaderConfiguration m_baseConfig;

    /// Initial RO colors before painting.
    Ra::Core::Vector4Array m_baseColors;

    /// Painted colors.
    Ra::Core::Vector4Array m_paintColors;
};

} // namespace MeshPaintPlugin

#endif
