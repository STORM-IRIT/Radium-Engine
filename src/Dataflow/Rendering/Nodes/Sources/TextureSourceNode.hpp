#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Node.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API TextureSourceNode : public RenderingNode
{
  public:
    TextureSourceNode( const std::string& instanceName,
                       const Ra::Engine::Data::TextureParameters& texParams );

    void execute() override;
    void destroy() override;

    void resize( uint32_t width, uint32_t height ) override;
    static const std::string getTypename() { return "TextureSource"; }

  protected:
    TextureSourceNode( const std::string& instanceName,
                       const std::string& typeName,
                       const Ra::Engine::Data::TextureParameters& texParams );

  private:
    // TODO : editable parameter for TextureParameters ?
    Ra::Engine::Data::Texture* m_texture { nullptr };
};

class RA_DATAFLOW_API ColorTextureNode : public TextureSourceNode
{
  public:
    explicit ColorTextureNode( const std::string& name );
    static const std::string getTypename() { return "Color Texture"; }
};

class RA_DATAFLOW_API DepthTextureNode : public TextureSourceNode
{
  public:
    explicit DepthTextureNode( const std::string& name );
    static const std::string getTypename() { return "Depth Texture"; }
};

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
