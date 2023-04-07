#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Node.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#include <globjects/Framebuffer.h>
#include <globjects/State.h>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {
using namespace Ra::Dataflow::Core;
/**
 * \brief Simple render node : render only opaque objects with direct lighting.
 *
 */
class RA_DATAFLOW_API TransparentLocalLightingNode : public RenderingNode
{
  public:
    explicit TransparentLocalLightingNode( const std::string& name );

    void init() override;

    bool execute() override;

    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;

    bool initInternalShaders() override;

    void buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                               Ra::Engine::Rendering::RenderTechnique& rt ) const override;

    bool hasRenderTechnique() override { return true; }

    static const std::string getTypename() { return "Transparency Local Lighting Node"; }

  private:
    bool m_hasShaders { false };
    /// The accumulation texture
    Ra::Engine::Data::Texture* m_accumulationTexture { nullptr };

    /// The revealage texture
    Ra::Engine::Data::Texture* m_revealageTexture { nullptr };

    /// The framebuffer used to draw the transparent objects
    globjects::Framebuffer* m_oitFramebuffer { nullptr };

    /// The composite shader, owned by the shader manager
    const Ra::Engine::Data::ShaderProgram* m_shader { nullptr };

    /// The fullscreen quad to draw
    std::unique_ptr<Ra::Engine::Data::Displayable> m_quadMesh { nullptr };

    Ra::Engine::Data::Texture* m_colorTexture { nullptr };

    globjects::Framebuffer* m_framebuffer { nullptr };

    globjects::State* m_nodeState;
    globjects::State* m_composeState;

    PortIn<TextureType>* m_inColor { new PortIn<TextureType>( "color", this ) };
    PortIn<TextureType>* m_inDepth { new PortIn<TextureType>( "depth", this ) };

    PortIn<std::vector<RenderObjectPtrType>>* m_inObjects {
        new PortIn<std::vector<RenderObjectPtrType>>( "objects", this ) };
    PortIn<std::vector<LightPtrType>>* m_inLights {
        new PortIn<std::vector<LightPtrType>>( "lights", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortOut<TextureType>* m_outColor { new PortOut<TextureType>( "Beauty", this ) };
    PortOut<TextureType>* m_outRevealage { new PortOut<TextureType>( "revealage", this ) };
    PortOut<TextureType>* m_outAccumulation { new PortOut<TextureType>( "accum", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra