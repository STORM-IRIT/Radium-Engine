#include <Renderer/ForwardRenderer.hpp>

#include <Renderer/OpenGL.hpp>
#include <Renderer/Camera.hpp>
#include <Renderer/ShaderProgram.hpp>
#include <Renderer/ShaderProgramManager.hpp>

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
	: Renderer(width, height)
	, m_camera(nullptr)
	, m_shaderManager(nullptr)
	, m_passthroughShader(nullptr)
	, m_blinnPhongShader(nullptr)
	, m_quadShader(nullptr)
{
	std::string shaderPath("../Engine/Shaders");
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

void ForwardRenderer::render()
{
	GL_ASSERT(glDepthMask(GL_TRUE));
	GL_ASSERT(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL_ASSERT(glEnable(GL_DEPTH_TEST));
}

void ForwardRenderer::resize(uint width, uint height)
{
}

} // namespace Ra