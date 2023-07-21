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
 * \brief Render a scene so that only the emissivity is computed.
 * This node expect to have as input the depth buffer of the scene (from any depth-prepass) and a
 * color buffer with the background color set.
 * This node will replace the color ofe each pixel in the color buffer by the computed emissivity
 * for all visible fragments.
 * pixels not covered by a fragments are left unchanged.
 *
 */
class RA_DATAFLOW_API EmissivityNode : public RenderingNode
{
  public:
    explicit EmissivityNode( const std::string& name );

    void init() override;

    bool execute() override;

    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;

    void buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                               Ra::Engine::Rendering::RenderTechnique& rt ) const override;

    bool hasRenderTechnique() override { return true; }

    static const std::string getTypename() { return "Emissivity Render Node"; }

  private:
    // This texture is not owned by the node, it is just an alias of input/output color texture
    Ra::Engine::Data::Texture* m_colorTexture { nullptr };

    Ra::Engine::Data::Texture* m_blankAO { nullptr };

    globjects::Framebuffer* m_framebuffer { nullptr };

    globjects::State* m_nodeState;

    PortIn<std::vector<RenderObjectPtrType>>* m_inObjects {
        new PortIn<std::vector<RenderObjectPtrType>>( "objects", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortIn<TextureType>* m_inDepth { new PortIn<TextureType>( "depth", this ) };
    PortIn<TextureType>* m_inColor { new PortIn<TextureType>( "color", this ) };
    PortIn<TextureType>* m_inAo { new PortIn<TextureType>( "AO", this ) };

    PortOut<TextureType>* m_outColor { new PortOut<TextureType>( "Beauty", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
