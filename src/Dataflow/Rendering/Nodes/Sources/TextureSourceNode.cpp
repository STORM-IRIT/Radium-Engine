#include <Dataflow/Rendering/Nodes/Sources/TextureSourceNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

TextureSourceNode::TextureSourceNode( const std::string& instanceName,
                                      const Ra::Engine::Data::TextureParameters& texParams ) :
    TextureSourceNode( instanceName, getTypename(), texParams ) {}

TextureSourceNode::TextureSourceNode( const std::string& instanceName,
                                      const std::string& typeName,
                                      const Ra::Engine::Data::TextureParameters& texParams ) :
    RenderingNode( instanceName, typeName ) {
    if ( !m_texture ) { m_texture = new Ra::Engine::Data::Texture( texParams ); }

    auto portOut = new PortOut<TextureType>( "texture", this );
    addOutput( portOut, m_texture );
}

void TextureSourceNode::execute() {
    auto interface = static_cast<PortIn<TextureType>*>( m_interface[0] );
    auto output    = static_cast<PortOut<TextureType>*>( m_outputs[0].get() );
    if ( interface->isLinked() ) { m_texture = &interface->getData(); }
    output->setData( m_texture );
}

void TextureSourceNode::destroy() {
    delete m_texture;
}

void TextureSourceNode::resize( uint32_t width, uint32_t height ) {
    m_texture->resize( width, height );
}

ColorTextureNode::ColorTextureNode( const std::string& name ) :
    TextureSourceNode( name,
                       getTypename(),
                       Ra::Engine::Data::TextureParameters { name + " (Color)",
                                                             gl::GL_TEXTURE_2D,
                                                             1,
                                                             1,
                                                             1,
                                                             gl::GL_RGBA,
                                                             gl::GL_RGBA32F,
                                                             gl::GL_FLOAT,
                                                             gl::GL_CLAMP_TO_EDGE,
                                                             gl::GL_CLAMP_TO_EDGE,
                                                             gl::GL_CLAMP_TO_EDGE,
                                                             gl::GL_LINEAR,
                                                             gl::GL_LINEAR,
                                                             nullptr } ) {}

DepthTextureNode::DepthTextureNode( const std::string& name ) :
    TextureSourceNode( name,
                       getTypename(),
                       Ra::Engine::Data::TextureParameters { name + " (Depth)",
                                                             gl::GL_TEXTURE_2D,
                                                             1,
                                                             1,
                                                             1,
                                                             gl::GL_DEPTH_COMPONENT,
                                                             gl::GL_DEPTH_COMPONENT24,
                                                             gl::GL_UNSIGNED_INT,
                                                             gl::GL_CLAMP_TO_EDGE,
                                                             gl::GL_CLAMP_TO_EDGE,
                                                             gl::GL_CLAMP_TO_EDGE,
                                                             gl::GL_NEAREST,
                                                             gl::GL_NEAREST,
                                                             nullptr } ) {}

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
