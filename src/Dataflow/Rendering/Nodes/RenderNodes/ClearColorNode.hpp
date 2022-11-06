#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Node.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

#include <globjects/Framebuffer.h>

namespace Ra {

namespace Engine {
namespace Data {
class EnvironmentTexture;
}
} // namespace Engine
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Dataflow::Core;
using namespace Ra::Engine::Data;
using EnvironmentType = std::shared_ptr<EnvironmentTexture>;

/**
 * \brief Initialize the given image either using a constant color or by rendering a sky box.
 *
 */
class RA_DATAFLOW_API ClearColorNode : public RenderingNode
{
  public:
    explicit ClearColorNode( const std::string& name );

    void init() override;
    bool execute() override;
    void destroy() override;

    void resize( uint32_t, uint32_t ) override {}

    static const std::string getTypename() { return "Clear Color Pass"; }

  protected:
    void toJsonInternal( nlohmann::json& data ) const override;
    bool fromJsonInternal( const nlohmann::json& data ) override;

  private:
    Ra::Engine::Data::Texture* m_colorTexture { nullptr };
    globjects::Framebuffer* m_framebuffer { nullptr };

    ColorType m_editableClearColor { ColorType::Grey( 0.12 ) };

    PortIn<TextureType>* m_portInColorTex {
        new PortIn<TextureType>( "colorTextureToClear", this ) };
    PortIn<ColorType>* m_portInClearColor { new PortIn<ColorType>( "clearColor", this ) };
    PortIn<EnvironmentType>* m_portInEnvmap { new PortIn<EnvironmentType>( "environment", this ) };
    PortIn<CameraType>* m_portInCamera { new PortIn<CameraType>( "cameras", this ) };
    PortOut<TextureType>* m_portOutColorTex { new PortOut<TextureType>( "image", this ) };
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
