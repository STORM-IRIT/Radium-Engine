#include <Engine/Renderer/Renderers/PointCloudRenderer.hpp>

#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Renderers/DebugRender.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

namespace Ra {
namespace Engine {

namespace {
    const GLenum buffers[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7
    };
}

PointCloudRenderer::PointCloudRenderer(uint width, uint height) :
    Renderer(width, height),
    m_brushRadius(0)
{
}

PointCloudRenderer::~PointCloudRenderer()
{
    ShaderProgramManager::destroyInstance();
}

////////////////////////////////////////////////////////////////////////////////
// Radium rendering
////////////////////////////////////////////////////////////////////////////////

std::string PointCloudRenderer::getRendererName() const
{
    return std::string("Point Cloud Renderer");
}

void PointCloudRenderer::initializeInternal()
{
    CORE_ASSERT(checkOpenGLContext(), "OpenGL context missing");

    initBuffers();
    initShaders();

    DebugRender::createInstance();

    // require at least one light
    m_light = std::make_shared<DirectionalLight>();
}

void PointCloudRenderer::resizeInternal()
{
    m_texture->Generate(m_width, m_height, GL_DEPTH_COMPONENT);
    m_accum->Generate(m_width, m_height, GL_RGBA);
    m_revealage->Generate(m_width, m_height, GL_RGBA);
    m_vertexPickingTex->Generate(m_width, m_height, GL_RGBA_INTEGER);

    m_fbo->bind();
        m_fbo->setSize(m_width, m_height);
        m_fbo->attachTexture(GL_DEPTH_ATTACHMENT , m_texture.get());
        m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get());
        m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_vertexPickingTex.get());
        m_fbo->check();
    m_fbo->unbind(true);

    m_oitFbo->bind();
        m_oitFbo->setSize(m_width, m_height);
        m_oitFbo->attachTexture(GL_DEPTH_ATTACHMENT , m_texture.get());
        m_oitFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_accum.get());
        m_oitFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_revealage.get());
        m_oitFbo->check();
    m_oitFbo->unbind( true );

    GL_CHECK_ERROR;

    // Reset framebuffer state
    GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

    GL_ASSERT( glDrawBuffer( GL_BACK ) );
    GL_ASSERT( glReadBuffer( GL_BACK ) );
}

void PointCloudRenderer::updateStepInternal(const RenderData& renderData)
{
    doVertexPicking();

    m_meshRO.clear();
    m_pointCloudRO.clear();

    m_roMgr->getRenderObjectsByType(renderData, m_meshRO,
                                    RenderObjectType::Mesh);
    m_roMgr->getRenderObjectsByType(renderData, m_pointCloudRO,
                                    RenderObjectType::PointCloud);

    for(auto& ro : m_meshRO)
        ro->updateGL();

    for(auto& ro : m_pointCloudRO)
        ro->updateGL();
}

void PointCloudRenderer::renderInternal(const RenderData& renderData)
{
    // clear ===================================================================
    m_fbo->useAsTarget(m_width, m_height);
    {
        GL_ASSERT( glDepthMask( GL_TRUE ) );
        GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
        GL_ASSERT( glDrawBuffers( 2, buffers ) );

        const float one = 1.0;
        int clearPicking[] = { -1, -1, -1, -1 };

        GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &one ) );
        GL_ASSERT( glClearBufferfv( GL_COLOR, 0, m_backgroundColor.data() ) );
        GL_ASSERT( glClearBufferiv( GL_COLOR, 1, clearPicking ) );
    }
    m_fbo->unbind();
    m_oitFbo->useAsTarget();
    {
        Core::Vector3 clearZeros(0,0,0);
        Core::Vector3 clearOnes(1,1,1);

        GL_ASSERT( glDrawBuffers(2, buffers) );
        GL_ASSERT( glClearBufferfv(GL_COLOR, 0, clearZeros.data()) );
        GL_ASSERT( glClearBufferfv(GL_COLOR, 1, clearOnes.data()) );
    }
    m_oitFbo->unbind();

    // Opaque rendering ========================================================
    bool opaque = m_mainTransparency>=0.99;
    if(opaque)
    {
        m_fbo->bind();
        {
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glDrawBuffers(2, buffers) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            if(m_culling)
                GL_ASSERT( glEnable( GL_CULL_FACE ) );

            RenderParameters params;
            m_light->getRenderParameters(params);
            params.addParameter("radius", m_pointSize);
            params.addParameter("flatShading", (float)m_flatShading);
            params.addParameter("lightingFactor", m_lighting);
            params.addParameter("threshold", m_threshold);

            for(const auto& ro : m_meshRO)
                if( ro->isVisible() )
                    ro->render(params, renderData);

            for(const auto& ro : m_pointCloudRO)
                if( ro->isVisible() )
                    ro->render(params, renderData);
        }
        m_fbo->unbind();
    }

    // 1st pass OIT rendering ==================================================
    m_oitFbo->useAsTarget();
    {
        GL_ASSERT( glDepthMask( GL_FALSE ) );
        GL_ASSERT( glDrawBuffers(2, buffers) );

        GL_ASSERT( glDepthFunc(GL_LESS) );
        GL_ASSERT( glEnable(GL_BLEND) );
        GL_ASSERT( glEnable(GL_PRIMITIVE_RESTART) );
        GL_ASSERT( glPrimitiveRestartIndex(Mesh::RESTART_INDEX) );

        GL_ASSERT( glBlendEquation(GL_FUNC_ADD) );
        GL_ASSERT( glBlendFunci(0, GL_ONE, GL_ONE) );
        GL_ASSERT( glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA) );

        RenderParameters params;
        params.addParameter("radius", m_pointSize);
        params.addParameter("lineWidth", m_lineWidth);
        params.addParameter("mainTransparency", m_mainTransparency);
        params.addParameter("lineTransparency", m_lineTransparency);
        params.addParameter("vectorLength", m_vectorLength);
        m_light->getRenderParameters(params);

        if(m_culling)
            GL_ASSERT( glEnable( GL_CULL_FACE ) );

        for (const auto& obj : m_fancyRenderObjects)
            if( obj->isVisible() )
                obj->render(params, renderData);

        GL_ASSERT( glDisable( GL_CULL_FACE ) );

        if(!opaque)
        {
            const ShaderProgram* shader;

            // mesh
            shader = m_shaderMgr->getShaderProgram("OITLitMesh");
            shader->bind();
            shader->setUniform("flatShading", (float)m_flatShading);
            for(const auto& ro : m_meshRO)
            {
                if(ro->isVisible())
                {
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );
                    shader->setUniform( "transform.worldNormal", N );
                    params.bind(shader);

                    ro->getMesh()->render();
                }
            }
            shader->unbind();

            // point cloud
            shader = m_shaderMgr->getShaderProgram("OITLitPointCloud");
            shader->bind();
            for(const auto& ro : m_pointCloudRO)
            {
                if(ro->isVisible())
                {
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );
                    shader->setUniform( "transform.worldNormal", N );
                    params.bind(shader);

                    ro->getMesh()->render();
                }
            }
            shader->unbind();
        }

        GL_ASSERT( glDisable(GL_PRIMITIVE_RESTART) );
    }
    m_oitFbo->unbind();

    // 2nd pass OIT rendering ==================================================
    m_fbo->useAsTarget();
    {
        GL_ASSERT( glDrawBuffers(1, buffers) );
        GL_ASSERT( glDepthMask( GL_FALSE ) );
        GL_ASSERT( glDepthFunc(GL_ALWAYS) );
        GL_ASSERT( glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA) );
        GL_ASSERT( glDisable( GL_CULL_FACE ) );

        const ShaderProgram* shader = m_shaderMgr->getShaderProgram("OITCompose");
        shader->bind();
        shader->setUniform("accumTexture", m_accum.get(), 0);
        shader->setUniform("revealageTexture", m_revealage.get(), 1);

        m_quadMesh->render();
    }
    m_fbo->unbind();

    // Wireframe rendering =====================================================
    if (m_wireframe)
    {
        m_fbo->useAsTarget();
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(1.f);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.0f, -1.1f);

            // Light pass
//            GL_ASSERT( glDepthFunc( GL_EQUAL ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );

            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

            GL_ASSERT( glDrawBuffers( 1, buffers) );  // Draw color texture

            RenderParameters params;
            m_light->getRenderParameters(params);

            const ShaderProgram* shader = m_shaderMgr->getShaderProgram("Mesh");
            shader->bind();

            for (const auto& ro : m_meshRO)
            {
                if(ro->isVisible())
                {
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );
                    shader->setUniform( "transform.worldNormal", N );
                    params.bind(shader);

                    ro->getMesh()->render();
                }
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
        m_fbo->unbind();
    }
}

void PointCloudRenderer::postProcessInternal(const RenderData& renderData)
{
}

void PointCloudRenderer::debugInternal(const RenderData& renderData)
{
    GL_ASSERT( glDisable( GL_BLEND ) );
    GL_ASSERT( glDepthMask( GL_FALSE ) );
    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    GL_ASSERT( glDepthFunc( GL_LESS ) );

    m_fbo->useAsTarget( m_width, m_height );
    {
        glDrawBuffers(1, buffers);

        glViewport(0, 0, m_width, m_height);

        if ( m_drawDebug )
        {
            for ( const auto& ro : m_debugRenderObjects )
            {
                 ro->render(RenderParameters{}, renderData);
            }

            DebugRender::getInstance()->render(renderData.viewMatrix,
                                               renderData.projMatrix);
        }
    }
    m_fbo->unbind();
}

void PointCloudRenderer::uiInternal(const RenderData& renderData)
{
    if(m_brushRadius==0)
        return;

    // draw brush circle
    m_fbo->useAsTarget( m_width, m_height );
    {
        GL_ASSERT( glDrawBuffers( 1, buffers) );
        GL_ASSERT( glDisable( GL_BLEND ) );
        GL_ASSERT( glDisable( GL_DEPTH_TEST ) );

        const ShaderProgram* shader = m_shaderMgr->getShaderProgram("CircleBrush");
        shader->bind();
        shader->setUniform("mousePosition", m_mousePosition);
        shader->setUniform("brushRadius", m_brushRadius);
        shader->setUniform("dim", Core::Vector2(m_width,m_height));

        m_quadMesh->render();
    }
    m_fbo->unbind();
}

////////////////////////////////////////////////////////////////////////////////
// Internal
////////////////////////////////////////////////////////////////////////////////

void PointCloudRenderer::initBuffers()
{
    m_fbo.reset(new FBO(FBO::Component_Color | FBO::Component_Depth,
                        m_width, m_height));
    m_oitFbo.reset(new FBO(FBO::Component_Color | FBO::Component_Depth,
                           m_width, m_height));

    m_texture.reset(new Texture("Pointy Depth"));
    m_accum.reset(new Texture("OIT Accum"));
    m_revealage.reset(new Texture("OIT Revealage"));
    m_vertexPickingTex.reset(new  Texture("Vertex picking"));

    m_texture->internalFormat = GL_DEPTH_COMPONENT24;
    m_texture->dataType       = GL_UNSIGNED_INT;

    m_accum->internalFormat = GL_RGBA32F;
    m_accum->dataType = GL_FLOAT;

    m_revealage->internalFormat = GL_RGBA32F;
    m_revealage->dataType = GL_FLOAT;

    m_vertexPickingTex->internalFormat = GL_RGBA32I;
    m_vertexPickingTex->dataType = GL_INT;
    m_vertexPickingTex->minFilter = GL_NEAREST;
    m_vertexPickingTex->magFilter = GL_NEAREST;

    m_secondaryTextures["Point Depth"] = m_texture.get();
    m_secondaryTextures["OIT Accum"] = m_accum.get();
    m_secondaryTextures["OIT Revealage"] = m_revealage.get();
    m_secondaryTextures["Vertex Picking"] = m_vertexPickingTex.get();
}

void PointCloudRenderer::initShaders()
{
    ShaderConfiguration configPointCloud("OITLitPointCloud");
    configPointCloud.addShader(ShaderType_VERTEX,   "Shaders/PointCloud/Basic.vert.glsl");
    configPointCloud.addShader(ShaderType_GEOMETRY, "Shaders/PointCloud/PointCloud.geom.glsl");
    configPointCloud.addShader(ShaderType_FRAGMENT, "Shaders/PointCloud/OITLitBasic.frag.glsl");
    ShaderConfigurationFactory::addConfiguration(configPointCloud);
    m_shaderMgr->addShaderProgram(configPointCloud);

    ShaderConfiguration configMesh("OITLitMesh");
    configMesh.addShader(ShaderType_VERTEX,   "Shaders/PointCloud/Basic.vert.glsl");
    configMesh.addShader(ShaderType_GEOMETRY, "Shaders/PointCloud/Mesh.geom.glsl");
    configMesh.addShader(ShaderType_FRAGMENT, "Shaders/PointCloud/OITLitBasic.frag.glsl");
    ShaderConfigurationFactory::addConfiguration(configMesh);
    m_shaderMgr->addShaderProgram(configMesh);

    ShaderConfiguration configLineComposeOIT("OITCompose");
    configLineComposeOIT.addShader(ShaderType_VERTEX,   "Shaders/Basic2D.vert.glsl");
    configLineComposeOIT.addShader(ShaderType_FRAGMENT, "Shaders/PointCloud/OITCompose.frag.glsl");
    ShaderConfigurationFactory::addConfiguration(configLineComposeOIT);
    m_shaderMgr->addShaderProgram(configLineComposeOIT);

    ShaderConfiguration configCircleBrush("CircleBrush");
    configCircleBrush.addShader(ShaderType_VERTEX,   "Shaders/Basic2D.vert.glsl");
    configCircleBrush.addShader(ShaderType_FRAGMENT, "Shaders/PointCloud/CircleBrush.frag.glsl");
    ShaderConfigurationFactory::addConfiguration(configCircleBrush);
    m_shaderMgr->addShaderProgram(configCircleBrush);
}

////////////////////////////////////////////////////////////////////////////////
// Picking
////////////////////////////////////////////////////////////////////////////////

void PointCloudRenderer::doVertexPicking()
{
    m_vertexPickingResult.reserve(m_vertexPickingQuery.size()*m_brushRadius*m_brushRadius);
    int picking_result[8];

    m_fbo->bind();
    {
        GL_ASSERT( glReadBuffer( GL_COLOR_ATTACHMENT1 ) );

        for(const auto& query : m_vertexPickingQuery)
        {
            for(int i=-m_brushRadius; i<=+m_brushRadius; i+=3)
            {
                Scalar t = (Scalar)i/m_brushRadius;
                int h = std::sqrt(m_brushRadius*m_brushRadius-t*t);
                for(int j=-h; j<=+h; j+=3)
                {
                    GL_ASSERT( glReadPixels(query.x+i, m_height-query.y-j,
                                             1, 1, GL_RGBA_INTEGER, GL_INT, picking_result ) );
                    m_vertexPickingResult.emplace_back(picking_result[0], query.add);
                }
            }
        }
    }
    m_fbo->unbind();

    m_vertexPickingQuery.clear();
}

} // namespace Engine
} // namespace Ra
