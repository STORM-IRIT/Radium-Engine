#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#include <globjects/Framebuffer.h>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {
using namespace Ra::Dataflow::Core;
using namespace Ra::Engine::Data;

/**
 * \brief compute antialiasing in image space from color buffer.
 *
 */
class RA_DATAFLOW_API FxaaNode : public RenderingNode
{
  public:
    explicit FxaaNode( const std::string& name );

    void init() override;
    bool execute() override;
    void destroy() override;
    void resize( uint32_t width, uint32_t height ) override;
    bool initInternalShaders() override;

    static const std::string getTypename() { return "FXAA Node"; }

  private:
    Ra::Engine::Data::Texture* m_colorTexture { nullptr };

    globjects::Framebuffer* m_framebuffer { nullptr };
    std::unique_ptr<Ra::Engine::Data::Displayable> m_quadMesh { nullptr };
    const Ra::Engine::Data::ShaderProgram* m_shader { nullptr };

    PortIn<TextureType>* m_inColor { new PortIn<TextureType>( "color", this ) };
    PortOut<TextureType>* m_outColor { new PortOut<TextureType>( "Beauty", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
