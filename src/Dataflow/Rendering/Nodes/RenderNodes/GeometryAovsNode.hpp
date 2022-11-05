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
 * \brief Render a scene into several geometry buffers.
 *   - Image space fragment detph
 *   - World space fragment normal
 *   - World space fragment position
 *
 */
class RA_DATAFLOW_API GeometryAovsNode : public RenderingNode
{
  public:
    explicit GeometryAovsNode( const std::string& name );

    void init() override;

    bool execute() override;

    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;

    void buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                               Ra::Engine::Rendering::RenderTechnique& rt ) const override;

    bool hasRenderTechnique() override { return true; }

    static const std::string getTypename() { return "Geometry AOVs"; }

  private:
    Ra::Engine::Data::Texture* m_depthTexture { nullptr };
    Ra::Engine::Data::Texture* m_posInWorldTexture { nullptr };
    Ra::Engine::Data::Texture* m_normalInWorldTexture { nullptr };

    globjects::Framebuffer* m_framebuffer { nullptr };

    globjects::State* m_nodeState;

    PortIn<std::vector<RenderObjectPtrType>>* m_inObjects {
        new PortIn<std::vector<RenderObjectPtrType>>( "objects", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortOut<TextureType>* m_outDepthTex { new PortOut<TextureType>( "depth", this ) };
    PortOut<TextureType>* m_outPosInWorldTex { new PortOut<TextureType>( "world pos", this ) };
    PortOut<TextureType>* m_outNormalInWorldTex {
        new PortOut<TextureType>( "world normal", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
