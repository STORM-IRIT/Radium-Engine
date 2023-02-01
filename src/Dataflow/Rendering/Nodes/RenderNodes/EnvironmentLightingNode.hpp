#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#include <Engine/Data/EnvironmentTexture.hpp>

#include <globjects/Framebuffer.h>
#include <globjects/State.h>

namespace Ra {
namespace Dataflow {
namespace Rendering {
using namespace Ra::Engine::Data;

// This should be made unique instead of duplicated in EnvMapSource
using EnvmapPtrType = std::shared_ptr<EnvironmentTexture>;

namespace Nodes {
using namespace Ra::Dataflow::Core;
/**
 * \brief Render a scene using an environment map as light source.
 *
 */
class RA_DATAFLOW_API EnvironmentLightingNode : public RenderingNode
{
  public:
    explicit EnvironmentLightingNode( const std::string& name );

    void init() override;

    bool execute() override;

    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;

    void buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                               Ra::Engine::Rendering::RenderTechnique& rt ) const override;

    bool hasRenderTechnique() override { return true; }

    static const std::string getTypename() { return "Env Lighting Node"; }

  private:
    Ra::Engine::Data::Texture* m_colorTexture;
    Ra::Engine::Data::Texture* m_blankAO { nullptr };

    globjects::Framebuffer* m_framebuffer { nullptr };

    globjects::State* m_nodeState;

    PortIn<TextureType>* m_inColor { new PortIn<TextureType>( "color", this ) };
    PortIn<TextureType>* m_inDepth { new PortIn<TextureType>( "depth", this ) };
    PortIn<TextureType>* m_inAo { new PortIn<TextureType>( "AO", this ) };
    PortIn<EnvmapPtrType>* m_inEnvironment { new PortIn<EnvmapPtrType>( "envmap", this ) };
    PortIn<std::vector<RenderObjectPtrType>>* m_inObjects {
        new PortIn<std::vector<RenderObjectPtrType>>( "objects", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortOut<TextureType>* m_outColor { new PortOut<TextureType>( "Beauty", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
