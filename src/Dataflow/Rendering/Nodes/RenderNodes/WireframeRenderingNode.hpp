#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#include <globjects/Framebuffer.h>
#include <globjects/State.h>

namespace Ra {
namespace Dataflow {
namespace Rendering {

namespace Nodes {
using namespace Ra::Dataflow::Core;
/**
 * \brief Render a scene using an environment map as light source.
 *
 */
class RA_DATAFLOW_API WireframeRenderingNode : public RenderingNode
{
  public:
    explicit WireframeRenderingNode( const std::string& name );

    void init() override;

    bool execute() override;

    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;

    void buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                               Ra::Engine::Rendering::RenderTechnique& rt ) const override;

    bool hasRenderTechnique() override { return true; }

    static const std::string getTypename() { return "Wireframe Node"; }

  protected:
    void toJsonInternal( nlohmann::json& data ) const override { data["activated"] = m_activate; }

    bool fromJsonInternal( const nlohmann::json& data ) override {
        if ( data.contains( "activated" ) ) { m_activate = data["activated"]; }
        else { m_activate = false; }
        return true;
    }

  private:
    Ra::Engine::Data::Texture* m_colorTexture;

    globjects::Framebuffer* m_framebuffer { nullptr };

    globjects::State* m_nodeState;

    using WireMap = std::map<Ra::Engine::Rendering::RenderObject*,
                             std::shared_ptr<Ra::Engine::Data::Displayable>>;
    mutable WireMap m_wireframes;

    uint m_wireframeWidth { 0 };
    uint m_wireframeHeight { 0 };

    bool m_activate { false };

    EditableParameter<bool>* m_editableActivate {
        new EditableParameter( "activated", m_activate ) };

    PortIn<TextureType>* m_inColor { new PortIn<TextureType>( "color", this ) };
    PortIn<TextureType>* m_inDepth { new PortIn<TextureType>( "depth", this ) };
    PortIn<bool>* m_inActivated { new PortIn<bool>( "activate", this ) };
    PortIn<std::vector<RenderObjectPtrType>>* m_inObjects {
        new PortIn<std::vector<RenderObjectPtrType>>( "objects", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortOut<TextureType>* m_outColor { new PortOut<TextureType>( "Beauty", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
