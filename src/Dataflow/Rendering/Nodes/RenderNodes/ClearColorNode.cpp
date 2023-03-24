#include <Dataflow/Rendering/Nodes/RenderNodes/ClearColorNode.hpp>

#include <Dataflow/Core/EditableParameter.hpp>

#include <globjects/State.h>

#include <Engine/Data/EnvironmentTexture.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

using EnvironmentType = std::shared_ptr<EnvironmentTexture>;

ClearColorNode::ClearColorNode( const std::string& name ) : RenderingNode( name, getTypename() ) {
    addInput( m_portInColorTex );
    m_portInColorTex->mustBeLinked();
    addInput( m_portInClearColor );
    addInput( m_portInEnvmap );
    addInput( m_portInCamera );

    addOutput( m_portOutColorTex, m_colorTexture );

    auto editableColor = new EditableParameter( "clear color", m_editableClearColor );
    addEditableParameter( editableColor );
}

void ClearColorNode::init() {
    m_framebuffer = new globjects::Framebuffer();
}

void ClearColorNode::destroy() {
    delete m_framebuffer;
}

bool ClearColorNode::execute() {

    // Color texture
    m_colorTexture = &m_portInColorTex->getData();
    m_portOutColorTex->setData( m_colorTexture );

    // Clear color
    Scalar* clearColor = m_editableClearColor.data();
    if ( m_portInClearColor->isLinked() ) { clearColor = m_portInClearColor->getData().data(); }

    // Envmap
    EnvironmentTexture* envmap { nullptr };
    if ( m_portInEnvmap->isLinked() && m_portInCamera->isLinked() ) {
        envmap = m_portInEnvmap->getData().get();
    }

    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );
    const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0 };
    gl::glDrawBuffers( 1, buffers );
    gl::glDisable( gl::GL_BLEND );

    if ( envmap ) {
        gl::glDepthMask( gl::GL_FALSE );
        gl::glColorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
        gl::glDisable( gl::GL_DEPTH_TEST );
        envmap->render( m_portInCamera->getData(), false );
        gl::glDepthMask( gl::GL_TRUE );
        gl::glEnable( gl::GL_DEPTH_TEST );
    }
    else {
        clearColor[3] = 0_ra;
        gl::glClearBufferfv( gl::GL_COLOR, 0, clearColor );
    }

    m_framebuffer->detach( gl::GL_COLOR_ATTACHMENT0 );
    m_framebuffer->unbind();
    return true;
}

void ClearColorNode::toJsonInternal( nlohmann::json& data ) const {
    auto c = ColorType::linearRGBTosRGB( m_editableClearColor );
    std::array<Scalar, 3> color { { c.x(), c.y(), c.z() } };
    data["clearColor"] = color;
}

bool ClearColorNode::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "clearColor" ) ) {
        std::array<Scalar, 3> c = data["clearColor"];
        m_editableClearColor    = ColorType::sRGBToLinearRGB( ColorType( c[0], c[1], c[2] ) );
    }
    else {
        m_editableClearColor =
            ColorType::sRGBToLinearRGB( ColorType( 42, 42, 42 ) * 1_ra / 255_ra );
    }
    return true;
}

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
