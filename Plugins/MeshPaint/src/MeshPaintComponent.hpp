#ifndef MESHPAINTPLUGIN_MESHPAINTCOMPONENT_HPP
#define MESHPAINTPLUGIN_MESHPAINTCOMPONENT_HPP

#include <MeshPaintPluginMacros.hpp>

#include <Core/Geometry/TriangleMesh.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Core/Utils/TaskQueue.hpp>

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

class MESH_PAINT_PLUGIN_API MeshPaintComponent : public Ra::Engine::Component {
  public:
    MeshPaintComponent( const std::string& name, Ra::Engine::Entity* entity );
    virtual ~MeshPaintComponent();

    virtual void initialize() override;
    virtual void addTasks( Ra::Core::Utils::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& info );

    void setDataId( const std::string& id );

    void startPaint( bool on );
    void paintMesh( const Ra::Engine::Renderer::PickingResult& picking,
                    const Ra::Core::Math::Color& color );

  protected:
    // Geometry data
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Container::Index>::Getter m_renderObjectReader;

    // Data id for compoenent messenger
    std::string m_dataId;

    // Initial RO shader config when not painting
    Ra::Engine::ShaderConfiguration m_baseConfig;
    Ra::Core::Container::Vector4Array m_baseColors;
    Ra::Core::Container::Vector4Array m_paintColors;
};

} // namespace MeshPaintPlugin

#endif
