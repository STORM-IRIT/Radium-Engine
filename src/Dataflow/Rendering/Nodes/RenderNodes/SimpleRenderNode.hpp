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
class RA_DATAFLOW_API SimpleRenderNode : public RenderingNode
{
  public:
    explicit SimpleRenderNode( const std::string& name );

    void init() override;

    bool execute() override;

    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;

    void buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                               Ra::Engine::Rendering::RenderTechnique& rt ) const override;

    bool hasRenderTechnique() override { return true; }

    static const std::string getTypename() { return "Simple Render Node"; }

  private:
    Ra::Engine::Data::Texture* m_colorTexture { nullptr };
    Ra::Engine::Data::Texture* m_depthTexture { nullptr };

    Ra::Engine::Data::Texture* m_blankAO { nullptr };

    globjects::Framebuffer* m_framebuffer { nullptr };

    globjects::State* m_nodeState;

    PortIn<std::vector<RenderObjectPtrType>>* m_inObjects {
        new PortIn<std::vector<RenderObjectPtrType>>( "objects", this ) };
    PortIn<std::vector<LightPtrType>>* m_inLights {
        new PortIn<std::vector<LightPtrType>>( "lights", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortOut<TextureType>* m_outColorTex { new PortOut<TextureType>( "Beauty", this ) };
    PortOut<TextureType>* m_outDepthTex { new PortOut<TextureType>( "Depth AOV", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
