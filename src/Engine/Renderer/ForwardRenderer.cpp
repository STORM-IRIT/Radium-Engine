#include <Engine/Renderer/ForwardRenderer.hpp>

#include <Engine/Renderer/OpenGL.hpp>
#include <Engine/Renderer/Camera.hpp>
#include <Engine/Renderer/ShaderProgram.hpp>
#include <Engine/Renderer/ShaderProgramManager.hpp>

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

ForwardRenderer::ForwardRenderer()
	: ForwardRenderer(1, 1)
{
}

ForwardRenderer::ForwardRenderer(uint width, uint height)
    : RenderSystem(width, height)
	, m_camera(nullptr)
	, m_shaderManager(nullptr)
	, m_passthroughShader(nullptr)
	, m_blinnPhongShader(nullptr)
	, m_quadShader(nullptr)
{
    std::string shaderPath("../Shaders");
	std::string defaultShader("Default");

	// TODO (Charly) : Material Manager ?

	m_shaderManager = ShaderProgramManager::createInstance(shaderPath, defaultShader);
}

ForwardRenderer::~ForwardRenderer()
{
	ShaderProgramManager::destroyInstance();
}

void ForwardRenderer::initialize()
{
	initShaders();
	initBuffers();

	m_camera = std::make_shared<Camera>();
}

void ForwardRenderer::initShaders()
{
	m_passthroughShader = m_shaderManager->addShaderProgram("PassThrough");
	m_blinnPhongShader  = m_shaderManager->addShaderProgram("BlinnPhong");
	m_quadShader        = m_shaderManager->addShaderProgram("Quad");
}

void ForwardRenderer::initBuffers()
{
}

void ForwardRenderer::update()
{
	GL_ASSERT(glDepthMask(GL_TRUE));
	GL_ASSERT(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL_ASSERT(glEnable(GL_DEPTH_TEST));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glViewport(0, 0, m_width, m_height);
    GL_ASSERT(glClearColor(0.8, 0.1, 0.1, 1.0));

    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    for (const auto& c : m_components)
    {

    }
}

void ForwardRenderer::resize(uint width, uint height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);

    // Reset framebuffer state
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_ASSERT(glDrawBuffer(GL_BACK));
    GL_ASSERT(glReadBuffer(GL_BACK));
}

} // namespace Ra
