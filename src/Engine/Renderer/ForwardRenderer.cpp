#include <Engine/Renderer/ForwardRenderer.hpp>

#include <iostream>

#include <QtCore>

#include <Engine/Renderer/OpenGL.hpp>
#include <Engine/Renderer/Camera.hpp>
#include <Engine/Renderer/ShaderProgram.hpp>
#include <Engine/Renderer/ShaderProgramManager.hpp>
#include <Engine/Renderer/DrawableComponent.hpp>

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
    m_camera->setPosition(Vector3(0, 2, -5), Camera::ModeType::TARGET);
    m_camera->setTargetPoint(Vector3(0, 0, 0));
    m_camera->updateProjMatrix(m_width, m_height);
}

void ForwardRenderer::initShaders()
{
//	m_passthroughShader = m_shaderManager->addShaderProgram("PassThrough");
	m_blinnPhongShader  = m_shaderManager->addShaderProgram("BlinnPhong");
//	m_quadShader        = m_shaderManager->addShaderProgram("Quad");
}

void ForwardRenderer::initBuffers()
{
}

void ForwardRenderer::update()
{
    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    GL_ASSERT(glEnable(GL_DEPTH_TEST));

//    GL_ASSERT(glEnable(GL_CULL_FACE));
    GL_ASSERT(glDisable(GL_CULL_FACE));
//    GL_ASSERT(glCullFace(GL_BACK));

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDrawBuffer(GL_BACK);

    glViewport(0, 0, m_width, m_height);
    GL_ASSERT(glClearColor(0.1, 0.1, 0.1, 1.0));

    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    m_blinnPhongShader->bind();

    m_camera->updateViewMatrix();

    m_blinnPhongShader->setUniform("view", m_camera->getViewMatrix());
    m_blinnPhongShader->setUniform("proj", m_camera->getProjMatrix());

    for (const auto& c : m_components)
    {
        DrawableComponent* dc = static_cast<DrawableComponent*>(c.second);

        dc->setShaderProgram(m_blinnPhongShader);
        dc->update();
    }
}

void ForwardRenderer::resize(uint width, uint height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);

    m_camera->updateProjMatrix(m_width, m_height);

    // Reset framebuffer state
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_ASSERT(glDrawBuffer(GL_BACK));
    GL_ASSERT(glReadBuffer(GL_BACK));
}

bool ForwardRenderer::handleMouseEvent(const MouseEvent& event)
{
    switch (event.event)
    {
        case MouseEventType::MOUSE_RELEASED:
        {
            switch (event.button)
            {
                case MouseButton::MOUSE_LEFT_BUTTON:
                {
                    fprintf(stderr, "Left button released at (%.0f %.0f) - relative position is (%0.3f %0.3f)\n",
                           event.absoluteXPosition, event.absoluteYPosition, event.relativeXPosition, event.relativeYPosition);
                } break;

                case MouseButton::MOUSE_MIDDLE_BUTTON:
                {
                    fprintf(stderr, "Middle button released at (%.0f %.0f) - relative position is (%0.3f %0.3f)\n",
                           event.absoluteXPosition, event.absoluteYPosition, event.relativeXPosition, event.relativeYPosition);
                } break;

                case MouseButton::MOUSE_RIGHT_BUTTON:
                {
                    fprintf(stderr, "Right button released at (%.0f %.0f) - relative position is (%0.3f %0.3f)\n",
                           event.absoluteXPosition, event.absoluteYPosition, event.relativeXPosition, event.relativeYPosition);
                } break;
            }
        } break;
    }

    return false;
}

bool ForwardRenderer::handleKeyEvent(const KeyEvent &event)
{
    switch (event.key)
    {
        // Reload shaders on Ctrl+R
        case Qt::Key_R:
        {
            if (event.event == KeyEventType::KEY_RELEASED
                    && event.modifier & Modifier::CTRL_KEY)
            {
                ShaderProgramManager::getInstancePtr()->reloadAllShaderPrograms();
                return true;
            }
        } break;

        // Handle camera moving. Just an example.
        case Qt::Key_A:
        {
            printf("Moving the camera left.\n");
            return true;
        } break;

        case Qt::Key_D:
        {
            printf("Moving the camera right.\n");
            return true;
        } break;

        case Qt::Key_W:
        {
            printf("Moving the camera up.\n");
            return true;
        } break;

        case Qt::Key_S:
        {
            printf("Moving the camera down.\n");
            return true;
        } break;
    }

    return false;
}

} // namespace Ra
