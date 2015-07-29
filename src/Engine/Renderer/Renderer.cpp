#include <Engine/Renderer/Renderer.hpp>

#include <iostream>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>

namespace Ra
{

namespace
{
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

Engine::Renderer::Renderer(uint width, uint height)
    : m_engine(nullptr)
    , m_width(width)
    , m_height(height)
    , m_shaderManager(nullptr)
    , m_displayedTexture(nullptr)
    , m_displayedIsDepth(false)
    , m_depthAmbientShader(nullptr)
    , m_compositingShader(nullptr)
    , m_drawScreenShader(nullptr)
    , m_quadMesh(nullptr)
    , m_fbo(nullptr)
    , m_postprocessFbo(nullptr)
{
    m_time.start();
}

Engine::Renderer::~Renderer()
{
    ShaderProgramManager::destroyInstance();
}

void Engine::Renderer::initialize()
{
    std::string shaderPath("../Shaders");
    std::string defaultShader("Default");

    m_shaderManager = ShaderProgramManager::createInstance(shaderPath, defaultShader);
    m_textureManager = TextureManager::createInstance();

    initShaders();
    initBuffers();

    Core::TriangleMesh mesh;
    mesh.m_vertices.push_back({ Scalar(-1), Scalar(-1), Scalar(0) });
    mesh.m_vertices.push_back({ Scalar(-1), Scalar( 1), Scalar(0) });
    mesh.m_vertices.push_back({ Scalar( 1), Scalar( 1), Scalar(0) });
    mesh.m_vertices.push_back({ Scalar( 1), Scalar(-1), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });
    mesh.m_normals.push_back ({ Scalar( 0), Scalar( 0), Scalar(0) });

    mesh.m_triangles.push_back({ Core::TriangleIdx(0), Core::TriangleIdx(2), Core::TriangleIdx(1) });
    mesh.m_triangles.push_back({ Core::TriangleIdx(0), Core::TriangleIdx(3), Core::TriangleIdx(2) });

    m_quadMesh = new Mesh("quad");
    m_quadMesh->loadGeometry(mesh);
    m_quadMesh->updateGL();

    m_totalTime = 0.0;
}

void Engine::Renderer::initShaders()
{
    m_depthAmbientShader = m_shaderManager->addShaderProgram("DepthAmbientPass");
    m_compositingShader = m_shaderManager->addShaderProgram("Compose");
    m_drawScreenShader  = m_shaderManager->addShaderProgram("DrawScreen");
}

void Engine::Renderer::initBuffers()
{
    m_fbo = new FBO(FBO::Components(FBO::COLOR | FBO::DEPTH), m_width, m_height);
    m_postprocessFbo = new FBO(FBO::Components(FBO::COLOR), m_width, m_height);

    m_textures[TEXTURE_DEPTH]    = new Texture("Depth", GL_TEXTURE_2D);
    m_textures[TEXTURE_AMBIENT]  = new Texture("Ambient", GL_TEXTURE_2D);
    m_textures[TEXTURE_POSITION] = new Texture("Position", GL_TEXTURE_2D);
    m_textures[TEXTURE_NORMAL]   = new Texture("Normal", GL_TEXTURE_2D);
    m_textures[TEXTURE_PICKING]  = new Texture("Picking", GL_TEXTURE_2D);
    m_textures[TEXTURE_COLOR]    = new Texture("Color", GL_TEXTURE_2D);

    m_finalTexture = new Texture("Final", GL_TEXTURE_2D);

    resize(m_width, m_height);

    m_displayedTexture = m_finalTexture;
}

void Engine::Renderer::render(const RenderData& data)
{
    std::lock_guard<std::mutex> renderLock(m_renderMutex);

    std::vector<std::shared_ptr<Drawable>> drawables;
    if (m_engine != nullptr)
    {
        drawables = m_engine->getDrawableManager()->getDrawables();
    }

    saveExternalFBOInternal();
    m_totalTime = m_time.elapsed()/1000.f;

    updateDrawablesInternal(data, drawables);
    renderInternal(data, drawables);
    postProcessInternal(data, drawables);

    drawScreenInternal();
}

void Engine::Renderer::saveExternalFBOInternal()
{
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_qtPlz));
}

void Engine::Renderer::updateDrawablesInternal(const RenderData &renderData,
                                               const std::vector<std::shared_ptr<Drawable> > &drawables)
{
    CORE_UNUSED(renderData);

    for (auto& d : drawables)
    {
        d->updateGL();
    }
}

void Engine::Renderer::renderInternal(const RenderData& renderData,
                                      const std::vector<std::shared_ptr<Drawable>>& drawables)
{
    m_fbo->useAsTarget();

    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(1, 1, 1, 1));

    GL_ASSERT(glDrawBuffers(5, buffers));

    Core::Color clearColor(0.2, 0.2, 0.2, 1.0);
    Core::Color clearEmpty(0.0, 0.0, 0.0, 0.0);
    Core::Color clearPicking(1.0, 1.0, 1.0, 1.0);
    Scalar clearDepth = 1.0;

    GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearColor.data())); // Clear ambient
    GL_ASSERT(glClearBufferfv(GL_COLOR, 1, clearEmpty.data()));  // Clear position
    GL_ASSERT(glClearBufferfv(GL_COLOR, 2, clearEmpty.data()));  // Clear normal
    GL_ASSERT(glClearBufferfv(GL_COLOR, 3, clearPicking.data()));  // Clear picking
    GL_ASSERT(glClearBufferfv(GL_COLOR, 4, clearEmpty.data()));  // Clear color
    GL_ASSERT(glClearBufferfv(GL_DEPTH, 0, &clearDepth));       // Clear depth

    Core::Matrix4 view = renderData.viewMatrix;
    Core::Matrix4 proj = renderData.projMatrix;

    // Z + Ambient Prepass
    GL_ASSERT(glEnable(GL_DEPTH_TEST));
    GL_ASSERT(glDepthFunc(GL_LESS));
    GL_ASSERT(glDepthMask(GL_TRUE));

    GL_ASSERT(glDisable(GL_BLEND));

    GL_ASSERT(glDrawBuffers(4, buffers)); // Draw ambient, position, normal, picking

    m_depthAmbientShader->bind();

    for (const auto& d : drawables)
    {
        // Object ID
        int index = d->idx.getValue();
        Scalar r = Scalar((index & 0x000000FF) >> 0) / 255.0;
        Scalar g = Scalar((index & 0x0000FF00) >> 8) / 255.0;
        Scalar b = Scalar((index & 0x00FF0000) >> 16) / 255.0;

        m_depthAmbientShader->setUniform("objectId", Core::Vector3(r, g, b));
        d->draw(view, proj, m_depthAmbientShader);
    }

    // Light pass
    GL_ASSERT(glDepthFunc(GL_LEQUAL));
    GL_ASSERT(glDepthMask(GL_FALSE));

    GL_ASSERT(glEnable(GL_BLEND));
    GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));

    GL_ASSERT(glDrawBuffers(1, buffers + 4)); // Draw color texture

    if (m_lights.size() > 0)
    {
        for (const auto& l : m_lights)
        {
            for (const auto& d : drawables)
            {
                d->draw(view, proj, l);
            }
        }
    }
    else
    {
        DirectionalLight l;
        l.setDirection(Core::Vector3(0.3f, 1, 0));

        for (const auto& d : drawables)
        {
            d->draw(view, proj, &l);
        }
    }

    GL_ASSERT(glDisable(GL_BLEND));
    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glDepthFunc(GL_LESS));

    m_fbo->unbind();
}

void Engine::Renderer::postProcessInternal(const RenderData &renderData,
                                           const std::vector<std::shared_ptr<Drawable>>& drawables)
{
    CORE_UNUSED(renderData);
    CORE_UNUSED(drawables);

    m_postprocessFbo->useAsTarget(m_width, m_height);

    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(1, 1, 1, 1));

    GL_ASSERT(glClearColor(1.0, 0.0, 0.0, 0.0));
    GL_ASSERT(glClearDepth(0.0));
    m_postprocessFbo->clear(FBO::Components(FBO::COLOR | FBO::DEPTH));

    // FIXME(Charly): Do we really need to clear the depth buffer ?
    GL_ASSERT(glDrawBuffers(1, buffers));

    GL_ASSERT(glDepthFunc(GL_ALWAYS));
	
    m_compositingShader->bind();
    m_compositingShader->setUniform("ambient", m_textures[TEXTURE_AMBIENT], 0);
    m_compositingShader->setUniform("color", m_textures[TEXTURE_COLOR], 1);

    m_quadMesh->draw();

    GL_ASSERT(glDepthFunc(GL_LESS));

    m_postprocessFbo->unbind();
}

void Engine::Renderer::drawScreenInternal()
{
    if (m_qtPlz == 0)
    {
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        glDrawBuffer(GL_BACK);
    }
    else
    {
        GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, m_qtPlz));
        GL_ASSERT(glDrawBuffers(1, buffers));
    }

    GL_ASSERT(glClearColor(0.0, 0.0, 0.0, 0.0));
    // FIXME(Charly): Do we really need to clear the depth buffer ?
    GL_ASSERT(glClearDepth(1.0));
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    GL_ASSERT(glDepthFunc(GL_ALWAYS));

    GL_ASSERT(glViewport(0, 0, m_width, m_height));

    m_drawScreenShader->bind();
    m_drawScreenShader->setUniform("isDepthTexture", m_displayedIsDepth ? 1 : 0);
    m_drawScreenShader->setUniform("screenTexture", m_finalTexture, 0);

    m_drawScreenShader->setUniform("depth", m_textures[TEXTURE_DEPTH], 1);
    m_drawScreenShader->setUniform("ambient", m_textures[TEXTURE_AMBIENT], 2);
    m_drawScreenShader->setUniform("position", m_textures[TEXTURE_POSITION], 3);
    m_drawScreenShader->setUniform("normal", m_textures[TEXTURE_NORMAL], 4);
    m_drawScreenShader->setUniform("color", m_textures[TEXTURE_COLOR], 5);
    m_drawScreenShader->setUniform("picking", m_textures[TEXTURE_PICKING], 6);

    m_drawScreenShader->setUniform("totalTime", m_totalTime);
    m_quadMesh->draw();

	GL_ASSERT(glDepthFunc(GL_LESS));
}

void Engine::Renderer::resize(uint w, uint h)
{
    m_width = w;
    m_height = h;
    glViewport(0, 0, m_width, m_height);

    if (m_textures[TEXTURE_DEPTH]->getId() != 0)
    {
        m_textures[TEXTURE_DEPTH]->deleteGL();
    }
    if (m_textures[TEXTURE_AMBIENT]->getId() != 0)
    {
        m_textures[TEXTURE_AMBIENT]->deleteGL();
    }
    if (m_textures[TEXTURE_POSITION]->getId() != 0)
    {
        m_textures[TEXTURE_POSITION]->deleteGL();
    }
    if (m_textures[TEXTURE_NORMAL]->getId() != 0)
    {
        m_textures[TEXTURE_NORMAL]->deleteGL();
    }
    if (m_textures[TEXTURE_PICKING]->getId() != 0)
    {
        m_textures[TEXTURE_PICKING]->deleteGL();
    }
    if (m_textures[TEXTURE_COLOR]->getId() != 0)
    {
        m_textures[TEXTURE_COLOR]->deleteGL();
    }

    if (m_finalTexture->getId() != 0)
    {
        m_finalTexture->deleteGL();
    }

    m_textures[TEXTURE_DEPTH]->initGL(GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    m_textures[TEXTURE_DEPTH]->setFilter(GL_LINEAR, GL_LINEAR);
    m_textures[TEXTURE_DEPTH]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_textures[TEXTURE_AMBIENT]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_textures[TEXTURE_AMBIENT]->setFilter(GL_LINEAR, GL_LINEAR);
    m_textures[TEXTURE_AMBIENT]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_textures[TEXTURE_POSITION]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_textures[TEXTURE_POSITION]->setFilter(GL_LINEAR, GL_LINEAR);
    m_textures[TEXTURE_POSITION]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_textures[TEXTURE_NORMAL]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_textures[TEXTURE_NORMAL]->setFilter(GL_LINEAR, GL_LINEAR);
    m_textures[TEXTURE_NORMAL]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_textures[TEXTURE_PICKING]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_textures[TEXTURE_PICKING]->setFilter(GL_LINEAR, GL_LINEAR);
    m_textures[TEXTURE_PICKING]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_textures[TEXTURE_COLOR]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_textures[TEXTURE_COLOR]->setFilter(GL_LINEAR, GL_LINEAR);
    m_textures[TEXTURE_COLOR]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_finalTexture->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_finalTexture->setFilter(GL_LINEAR, GL_LINEAR);
    m_finalTexture->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_fbo->bind();
    m_fbo->setSize(w, h);
    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_textures[TEXTURE_DEPTH]);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[TEXTURE_AMBIENT]);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[TEXTURE_POSITION]);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[TEXTURE_NORMAL]);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT3, m_textures[TEXTURE_PICKING]);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT4, m_textures[TEXTURE_COLOR]);
    m_fbo->check();
    m_fbo->unbind(true);

    m_postprocessFbo->bind();
    m_postprocessFbo->setSize(w, h);
    m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_finalTexture);
    m_postprocessFbo->check();
    m_postprocessFbo->unbind(true);

    GL_CHECK_ERROR;

    // Reset framebuffer state
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    GL_ASSERT(glDrawBuffer(GL_BACK));
    GL_ASSERT(glReadBuffer(GL_BACK));
}

void Engine::Renderer::debugTexture(uint texIdx)
{
    if (texIdx > TEXTURE_COUNT)
    {
        m_displayedTexture = m_finalTexture;
        m_displayedIsDepth = false;
    }
    else
    {
        m_displayedTexture = m_textures[texIdx];
        m_displayedIsDepth = (texIdx == 0);
    }
}

void Engine::Renderer::reloadShaders()
{
    ShaderProgramManager::getInstancePtr()->reloadAllShaderPrograms();
}

int Engine::Renderer::checkPicking(Scalar x, Scalar y) const
{
    Core::Color color = m_textures[TEXTURE_PICKING]->getTexel(x, y);

    if (color == Core::Color(1.0, 1.0, 1.0, 1.0))
    {
        return -1;
    }
    color = color * 255;

    uint id = color.x() + color.y() * 256 + color.z() * 256 * 256;
    return id;

//    int index = d->idx.getValue();
}

} // namespace Ra
