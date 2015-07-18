#include <Engine/Renderer/ForwardRenderer.hpp>

#include <iostream>

#include <QtCore>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Shader/ShaderProgramManager.hpp>
#include <Engine/Renderer/Drawable/DrawableComponent.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>

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

Engine::ForwardRenderer::ForwardRenderer()
    : RenderSystem()
	, m_camera(nullptr)
	, m_shaderManager(nullptr)
{
}

Engine::ForwardRenderer::~ForwardRenderer()
{
    ShaderProgramManager::destroyInstance();
}

void Engine::ForwardRenderer::initializeGL(uint width, uint height)
{
    m_width = width;
    m_height = height;

    std::string shaderPath("../Shaders");
    std::string defaultShader("Default");

    m_shaderManager = ShaderProgramManager::createInstance(shaderPath, defaultShader);

	initShaders();
	initBuffers();

    m_camera = std::make_shared<Camera>();
    m_camera->setPosition(Core::Vector3(5, 5, -10), Camera::ModeType::TARGET);
    m_camera->setTargetPoint(Core::Vector3(0, 0, 0));
    m_camera->updateProjMatrix(m_width, m_height);
}

void Engine::ForwardRenderer::initShaders()
{
//	m_passthroughShader = m_shaderManager->addShaderProgram("PassThrough");
    m_shaderManager->addShaderProgram("BlinnPhong");
//	m_quadShader        = m_shaderManager->addShaderProgram("Quad");
}

void Engine::ForwardRenderer::initBuffers()
{
}

void Engine::ForwardRenderer::render()
{
    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    GL_ASSERT(glEnable(GL_DEPTH_TEST));
    GL_ASSERT(glDepthFunc(GL_LEQUAL));

    GL_ASSERT(glEnable(GL_CULL_FACE));
//    GL_ASSERT(glDisable(GL_CULL_FACE));
    GL_ASSERT(glCullFace(GL_BACK));

//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDrawBuffer(GL_BACK);

    glViewport(0, 0, m_width, m_height);
    GL_ASSERT(glClearColor(0.1, 0.1, 0.1, 1.0));

    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    m_camera->updateViewMatrix();

    Core::Matrix4 view = m_camera->getViewMatrix();
    Core::Matrix4 proj = m_camera->getProjMatrix();

    // FIXME(Charly): Add z-prepass

    GL_ASSERT(glEnable(GL_BLEND));
    GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
    GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));

    if (m_lights.size() > 0)
    {
        for (const auto& l : m_lights)
        {
            for (const auto& c : m_components)
            {
                DrawableComponent* dc = static_cast<DrawableComponent*>(c.second);
                dc->draw(view, proj, l);
            }
        }
    }
    else
    {
        DirectionalLight l;
        l.setDirection(Core::Vector3(0.3, -1, 0));

        for (const auto& c : m_components)
        {
            DrawableComponent* dc = static_cast<DrawableComponent*>(c.second);
            dc->draw(view, proj, &l);
        }
    }

    GL_ASSERT(glDisable(GL_BLEND));
}

void Engine::ForwardRenderer::resize(uint width, uint height)
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

bool Engine::ForwardRenderer::handleMouseEvent(const Core::MouseEvent& event)
{
    switch (event.event)
    {
        case Core::MouseEventType::MOUSE_RELEASED:
        {
            switch (event.button)
            {
                case Core::MouseButton::MOUSE_LEFT_BUTTON:
                {
                    fprintf(stderr, "Left button released at (%.0f %.0f) - relative position is (%0.3f %0.3f)\n",
                           event.absoluteXPosition, event.absoluteYPosition, event.relativeXPosition, event.relativeYPosition);
                } break;

                case Core::MouseButton::MOUSE_MIDDLE_BUTTON:
                {
                    fprintf(stderr, "Middle button released at (%.0f %.0f) - relative position is (%0.3f %0.3f)\n",
                           event.absoluteXPosition, event.absoluteYPosition, event.relativeXPosition, event.relativeYPosition);
                } break;

                case Core::MouseButton::MOUSE_RIGHT_BUTTON:
                {
                    fprintf(stderr, "Right button released at (%.0f %.0f) - relative position is (%0.3f %0.3f)\n",
                           event.absoluteXPosition, event.absoluteYPosition, event.relativeXPosition, event.relativeYPosition);
                } break;
            }
        } break;
    }

    return false;
}

bool Engine::ForwardRenderer::handleKeyEvent(const Core::KeyEvent &event)
{
    switch (event.key)
    {
        // Reload shaders on Ctrl+R
        case Qt::Key_R:
        {
            if (event.event == Core::KeyEventType::KEY_RELEASED
                    && event.modifier & Core::Modifier::CTRL_KEY)
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
