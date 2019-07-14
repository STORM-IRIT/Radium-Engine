#ifndef MESHPAINTPLUGIN_MESHPAINTCOMPONENT_HPP
#define MESHPAINTPLUGIN_MESHPAINTCOMPONENT_HPP

#include <MeshPaintPluginMacros.hpp>

#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/Utils/Attribs.hpp>
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
class Mesh;
} // namespace Engine
} // namespace Ra

namespace MeshPaintPlugin {

/*!
 * \brief The MeshPaintComponent class allows to paint over TriangleMeshes
 */
class MESH_PAINT_PLUGIN_API MeshPaintComponent : public Ra::Engine::Component
{
  public:
    MeshPaintComponent( const std::string& name, Ra::Engine::Entity* entity );
    ~MeshPaintComponent() override;

    virtual void initialize() override;
    virtual void addTasks( Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& info );

    void setDataId( const std::string& id );

    /// Colors are reset to their original value when calling startPaint( false )
    /// \warning Called for all the components of the scene !
    void startPaint( bool on );
    /// Overwrite original colors with current paint
    /// \warning Called for all the components of the scene !
    void bakePaintToDiffuse();
    void paintMesh( const Ra::Engine::Renderer::PickingResult& picking,
                    const Ra::Core::Utils::Color& color );

  protected:
    // Geometry data
    Ra::Engine::ComponentMessenger::CallbackTypes<Ra::Core::Utils::Index>::Getter
        m_renderObjectReader;
    Ra::Engine::Mesh* m_mesh;

    // Data id for compoenent messenger
    std::string m_dataId;

    // Initial RO shader config when not painting
    Ra::Engine::ShaderConfiguration m_baseConfig;
    Ra::Core::Vector4Array m_baseColors;
    bool m_isBaseColorValid{false};
    Ra::Core::Utils::AttribHandle<Ra::Core::Vector4> m_currentColorAttribHdl;
};

} // namespace MeshPaintPlugin

#endif
