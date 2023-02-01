#include <Dataflow/Rendering/Nodes/AntiAliasing/FxaaNode.hpp>

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/ShaderConfiguration.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

FxaaNode::FxaaNode( const std::string& name ) : RenderingNode( name, getTypename() ) {
    addInput( m_inColor );
    m_inColor->mustBeLinked();

    Ra::Engine::Data::TextureParameters colorTexParams = { "FXAA image",
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
                                                           nullptr };
    m_colorTexture = new Ra::Engine::Data::Texture( colorTexParams );

    addOutput( m_outColor, m_colorTexture );
}

void FxaaNode::init() {
    m_framebuffer = new globjects::Framebuffer();
}

void FxaaNode::destroy() {
    delete m_framebuffer;
    delete m_colorTexture;
}

void FxaaNode::resize( uint32_t width, uint32_t height ) {
    m_colorTexture->resize( width, height );
}

bool FxaaNode::execute() {
    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );
    const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0 };
    gl::glDrawBuffers( 1, buffers );
    gl::glDisable( gl::GL_DEPTH_TEST );
    gl::glDepthMask( gl::GL_FALSE );
    gl::glColorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    gl::glDisable( gl::GL_BLEND );

    float clearBlack[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearBlack );

    m_shader->bind();
    m_shader->setUniform( "tex1_sampler", &m_inColor->getData(), 0 );
    m_quadMesh->render( m_shader );
    m_framebuffer->unbind();
    return true;
}

bool FxaaNode::initInternalShaders() {
    if ( m_quadMesh == nullptr ) {
        Ra::Core::Geometry::TriangleMesh mesh =
            Ra::Core::Geometry::makeZNormalQuad( Ra::Core::Vector2( -1.f, 1.f ) );
        auto qm = std::make_unique<Ra::Engine::Data::Mesh>( "caller" );
        qm->loadGeometry( std::move( mesh ) );
        m_quadMesh = std::move( qm );
        m_quadMesh->updateGL();
    }

    if ( m_shader == nullptr ) {
        const std::string composeVertexShader { "layout (location = 0) in vec3 in_position;\n"
                                                "out vec2 varTexcoord;\n"
                                                "void main()\n"
                                                "{\n"
                                                "  gl_Position = vec4(in_position, 1.0);\n"
                                                "  varTexcoord = (in_position.xy + 1.0) * 0.5;\n"
                                                "}\n" };
        const std::string composeFragmentShader {
            "layout (location = 0) out vec4 out_tex;\n"
            "uniform sampler2D tex1_sampler;\n"
            "in vec2 varTexcoord;\n"
            "const float THRESHOLD = 0.0312;\n"
            "const float RELATIVE_THRESHOLD = 0.125;\n"
            "void main() {\n"
            "vec2 texSize = vec2(textureSize(tex1_sampler, 0));\n"
            "vec2 texelSize = 1.0 / texSize;\n"
            "vec3 n = texture(tex1_sampler, varTexcoord + (vec2(0.0, -1.0) * texelSize)).rgb;\n"
            "vec3 s = texture(tex1_sampler, varTexcoord + (vec2(0.0, 1.0) * texelSize)).rgb;\n"
            "vec3 e = texture(tex1_sampler, varTexcoord + (vec2(1.0, 0.0) * texelSize)).rgb;\n"
            "vec3 w = texture(tex1_sampler, varTexcoord + (vec2(-1.0, 0.0) * texelSize)).rgb;\n"
            "vec3 m = texture(tex1_sampler, varTexcoord).rgb;\n"
            "vec3 brightnessCoefficients = vec3(0.2126, 0.7152, 0.0722);\n"
            "float brightnessN = dot(n, brightnessCoefficients);\n"
            "float brightnessS = dot(s, brightnessCoefficients);\n"
            "float brightnessE = dot(e, brightnessCoefficients);\n"
            "float brightnessW = dot(w, brightnessCoefficients);\n"
            "float brightnessM = dot(m, brightnessCoefficients);\n"
            "float brightnessMin = min(brightnessM, min(min(brightnessN, brightnessS), "
            "min(brightnessE, brightnessW)));\n"
            "float brightnessMax = max(brightnessM, max(max(brightnessN, brightnessS), "
            "max(brightnessE, brightnessW)));\n"
            "float contrast = brightnessMax - brightnessMin;\n"
            "float threshold = max(THRESHOLD, RELATIVE_THRESHOLD * brightnessMax);\n"
            "if (contrast < threshold) {\n"
            "out_tex = vec4(m, 1.0);\n"
            "}\n"
            "else {\n"
            "vec3 nw = texture(tex1_sampler, varTexcoord + (vec2(-1.0, -1.0) * texelSize)).rgb;\n"
            "vec3 ne = texture(tex1_sampler, varTexcoord + (vec2(1.0, -1.0) * texelSize)).rgb;\n"
            "vec3 sw = texture(tex1_sampler, varTexcoord + (vec2(-1.0, 1.0) * texelSize)).rgb;\n"
            "vec3 se = texture(tex1_sampler, varTexcoord + (vec2(1.0, 1.0) * texelSize)).rgb;\n"
            "float brightnessNW = dot(nw, brightnessCoefficients);\n"
            "float brightnessNE = dot(ne, brightnessCoefficients);\n"
            "float brightnessSW = dot(sw, brightnessCoefficients);\n"
            "float brightnessSE = dot(se, brightnessCoefficients);\n"
            "float factor = 2 * (brightnessN + brightnessS + brightnessE + brightnessW);\n"
            "factor += (brightnessNW + brightnessNE + brightnessSW + brightnessSE);\n"
            "factor *= (1.0 / 12.0);\n"
            "factor = abs(factor - brightnessM);\n"
            "factor = clamp(factor / contrast, 0.0, 1.0);\n"
            "factor = smoothstep(0.0, 1.0, factor);\n"
            "factor = factor * factor;\n"
            "float horizontal = abs(brightnessN + brightnessS - (2 * brightnessM)) * 2 +\n"
            "abs(brightnessNE + brightnessSE - (2 * brightnessE)) +\n"
            "abs(brightnessNW + brightnessSW - (2 * brightnessW));\n"
            "float vertical = abs(brightnessE + brightnessW - (2 * brightnessM)) * 2 +\n"
            "abs(brightnessNE + brightnessSE - (2 * brightnessN)) +\n"
            "abs(brightnessNW + brightnessSW - (2 * brightnessS));\n"
            "bool isHorizontal = horizontal > vertical;\n"
            "float pixelStep = isHorizontal ? texelSize.y : texelSize.x;\n"
            "float posBrightness = isHorizontal ? brightnessS : brightnessE;\n"
            "float negBrightness = isHorizontal ? brightnessN : brightnessW;\n"
            "float posGradient = abs(posBrightness - brightnessM);\n"
            "float negGradient = abs(negBrightness - brightnessM);\n"
            "pixelStep *= (posGradient < negGradient) ? -1 : 1;\n"
            "vec2 blendUV = varTexcoord;\n"
            "if (isHorizontal) {\n"
            "blendUV.y = varTexcoord.y + (pixelStep * factor);\n"
            "}\n"
            "else {\n"
            "blendUV.x = varTexcoord.x + (pixelStep * factor); \n"
            "}\n"
            "out_tex = texture(tex1_sampler, blendUV);\n"
            "}\n"
            "}" };

        Ra::Engine::Data::ShaderConfiguration config { "ComposeMax" };
        config.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_VERTEX,
                                composeVertexShader );
        config.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT,
                                composeFragmentShader );
        auto added = m_shaderMngr->addShaderProgram( config );
        if ( added ) { m_shader = added.value(); }
    }
    return ( m_shader != nullptr && m_quadMesh != nullptr );
}

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
