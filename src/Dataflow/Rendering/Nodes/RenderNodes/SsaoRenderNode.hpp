#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#include <globjects/Framebuffer.h>
#include <globjects/State.h>

#include <Engine/Data/SphereSampler.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {
using namespace Ra::Dataflow::Core;
using namespace Ra::Engine::Data;

/**
 * \brief compute ssao in image space from world space geometry buffer.
 *
 */
class RA_DATAFLOW_API SsaoNode : public RenderingNode
{
  public:
    explicit SsaoNode( const std::string& name );

    void init() override;
    bool execute() override;
    void destroy() override;
    void resize( uint32_t width, uint32_t height ) override;
    bool initInternalShaders() override;

    static const std::string getTypename() { return "SSAO Node"; }

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    bool fromJsonInternal( const nlohmann::json& data ) override;

  private:
    bool m_hasShaders { false };

    static constexpr Scalar AO_DefaultRadius { 5_ra };
    static constexpr int AO_DefaultSamples { 64 };

    TextureType* m_rawAO { nullptr };
    TextureType* m_AO { nullptr };

    Scalar m_editableAORadius { AO_DefaultRadius };
    int m_editableSamples { AO_DefaultSamples };

    int m_currentSamples { AO_DefaultSamples };
    Scalar m_sceneDiag { 1.0 };

    std::unique_ptr<Ra::Engine::Data::Displayable> m_quadMesh { nullptr };
    std::unique_ptr<SphereSampler> m_sphereSampler { nullptr };

    const Ra::Engine::Data::ShaderProgram* m_shader { nullptr };
    const Ra::Engine::Data::ShaderProgram* m_blurShader { nullptr };

    globjects::Framebuffer* m_blurFramebuffer { nullptr };
    globjects::Framebuffer* m_framebuffer { nullptr };

    PortIn<TextureType>* m_inWorldPos { new PortIn<TextureType>( "worldPosition", this ) };
    PortIn<TextureType>* m_inWorldNormal { new PortIn<TextureType>( "worldNormal", this ) };
    PortIn<CameraType>* m_inCamera { new PortIn<CameraType>( "camera", this ) };

    PortIn<Scalar>* m_aoRadius { new PortIn<Scalar>( "radius", this ) };
    PortIn<int>* m_aoSamples { new PortIn<int>( "samples", this ) };

    PortOut<TextureType>* m_ssao { new PortOut<TextureType>( "ssao", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
