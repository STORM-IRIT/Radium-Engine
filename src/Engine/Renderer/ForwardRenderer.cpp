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
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>

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
    , m_depthShader(nullptr)
    , m_quadShader(nullptr)
    , m_quadMesh(nullptr)
    , m_fbo(nullptr)
    , m_displayedTexture(nullptr)
    , m_displayedIsDepth(false)
    , m_camRotateStarted(false)
    , m_camZoomStarted(false)
    , m_camPanStarted(false)
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

    m_camera = std::shared_ptr<Camera>( new Camera() );
    m_camera->setPosition(Core::Vector3(0, 2, -5), Camera::ModeType::TARGET);
    m_camera->setTargetPoint(Core::Vector3(0, 0, 0));
    m_camera->updateProjMatrix(m_width, m_height);

    std::string shaderPath("../Shaders");
    std::string defaultShader("Default");

    m_shaderManager = ShaderProgramManager::createInstance(shaderPath, defaultShader);

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
}

void Engine::ForwardRenderer::initShaders()
{
    m_depthShader = m_shaderManager->addShaderProgram("DepthPass");
    m_shaderManager->addShaderProgram("BlinnPhong");

    m_quadShader  = m_shaderManager->addShaderProgram("Quad");
}

void Engine::ForwardRenderer::initBuffers()
{
    m_fbo = new FBO(FBO::Components(FBO::COLOR | FBO::DEPTH), m_width, m_height);

    m_textures[TEXTURE_DEPTH] = new Texture("Depth", GL_TEXTURE_2D);
    m_textures[TEXTURE_COLOR] = new Texture("Color", GL_TEXTURE_2D);

    resize(m_width, m_height);

    m_displayedTexture = m_textures[TEXTURE_COLOR];
}

void Engine::ForwardRenderer::render()
{
    int qtFtw;
    GL_ASSERT(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &qtFtw));

    m_fbo->bind();
    GL_ASSERT(glDepthMask(GL_TRUE));
    GL_ASSERT(glColorMask(1, 1, 1, 1));

    GL_ASSERT(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
    GL_ASSERT(glClearDepth(1.0));
    m_fbo->clear(FBO::Components(FBO::COLOR | FBO::DEPTH));

    GL_ASSERT(glDrawBuffers(1, buffers));

    m_camera->updateViewMatrix();

    Core::Matrix4 view = m_camera->getViewMatrix();
    Core::Matrix4 proj = m_camera->getProjMatrix();

    // Z Prepass
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glColorMask(0, 0, 0, 0);
        glDepthMask(GL_TRUE);

        glDisable(GL_BLEND);

        m_depthShader->bind();

        for (const auto& c : m_components)
        {
            DrawableComponent* dc = static_cast<DrawableComponent*>(c.second);
            dc->draw(view, proj, m_depthShader);
        }
    }

    // Light passes
    if (1)
    {
        glDepthFunc(GL_LEQUAL);
        glColorMask(1, 1, 1, 1);
        glDepthMask(GL_FALSE);
//        glDisable(GL_DEPTH_TEST);

        GL_ASSERT(glEnable(GL_BLEND));
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
            l.setDirection(Core::Vector3(0.3f, 1, 0));

            for (const auto& c : m_components)
            {
                DrawableComponent* dc = static_cast<DrawableComponent*>(c.second);
                dc->draw(view, proj, &l);
            }
        }
    }

    m_fbo->unbind(true);

    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, qtFtw));
//    GL_ASSERT(glDrawBuffers(1, buffers));

    GL_ASSERT(glClearColor(0.0, 0.0, 0.0, 0.0));
    GL_ASSERT(glClearDepth(1.0));
    GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

//    m_fbo->unbind(true);
    GL_ASSERT(glViewport(0, 0, m_width, m_height));

    m_quadShader->bind();
    m_quadShader->setUniform("isDepthTexture", m_displayedIsDepth ? 1 : 0);
    m_quadShader->setUniform("zNear", m_camera->getZNear());
    m_quadShader->setUniform("zFar", m_camera->getZFar());
    m_quadShader->setUniform("depth", m_textures[TEXTURE_DEPTH], 0);
    m_quadShader->setUniform("color", m_textures[TEXTURE_COLOR], 1);
    m_quadMesh->draw();
}

void Engine::ForwardRenderer::resize(uint w, uint h)
{
    m_width = w;
    m_height = h;
    glViewport(0, 0, m_width, m_height);

    if (m_textures[TEXTURE_DEPTH]->getId() != 0)
    {
        m_textures[TEXTURE_DEPTH]->deleteGL();
    }
    if (m_textures[TEXTURE_COLOR]->getId() != 0)
    {
        m_textures[TEXTURE_COLOR]->deleteGL();
    }

    m_textures[TEXTURE_DEPTH]->initGL(GL_DEPTH_COMPONENT24, w, h, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    m_textures[TEXTURE_DEPTH]->setFilter(GL_NEAREST, GL_NEAREST);
    m_textures[TEXTURE_DEPTH]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_textures[TEXTURE_COLOR]->initGL(GL_RGBA32F, w, h, GL_RGBA, GL_FLOAT, nullptr);
    m_textures[TEXTURE_COLOR]->setFilter(GL_NEAREST, GL_NEAREST);
    m_textures[TEXTURE_COLOR]->setClamp(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

    m_fbo->bind();
    m_fbo->setSize(w, h);
    m_fbo->attachTexture(GL_DEPTH_ATTACHMENT, m_textures[TEXTURE_DEPTH]);
    m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[TEXTURE_COLOR]);
    m_fbo->check();
    m_fbo->unbind(true);

    GL_CHECK_ERROR;

    m_camera->updateProjMatrix(m_width, m_height);

    // Reset framebuffer state
    GL_ASSERT(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_ASSERT(glDrawBuffer(GL_BACK));
    GL_ASSERT(glReadBuffer(GL_BACK));
}

void Engine::ForwardRenderer::debugTexture(uint texIdx)
{
    if (texIdx > TEXTURE_COUNT)
    {
        return;
    }

    m_displayedTexture = m_textures[texIdx];
    m_displayedIsDepth = (texIdx == 0);
}

bool Engine::ForwardRenderer::handleMouseEvent(const Core::MouseEvent& event)
{
    switch (event.event)
    {
        case Core::MouseEventType::RA_MOUSE_PRESSED:
        {
            switch (event.button)
            {
                case Core::MouseButton::RA_MOUSE_LEFT_BUTTON:
                {
                    m_camRotateStarted = true;
                    m_lastMouseX = event.relativeXPosition;
                    m_lastMouseY = event.relativeYPosition;

                    return true;
                } break;

                case Core::MouseButton::RA_MOUSE_MIDDLE_BUTTON:
                {
                    m_camZoomStarted = true;
                    m_lastMouseX = event.relativeXPosition;
                    m_lastMouseY = event.relativeYPosition;

                    return true;
                } break;

                case Core::MouseButton::RA_MOUSE_RIGHT_BUTTON:
                {
                    m_camPanStarted = true;
                    m_lastMouseX = event.relativeXPosition;
                    m_lastMouseY = event.relativeYPosition;

                    return true;
                } break;
            }
        } break;

        case Core::MouseEventType::RA_MOUSE_MOVED:
        {
            bool handled = false;

            Scalar dx = event.relativeXPosition - m_lastMouseX;
            Scalar dy = event.relativeYPosition - m_lastMouseY;

            if (m_camRotateStarted)
            {
                m_camera->rotateRight(dx);
                m_camera->rotateUp(dy);

                handled = true;
            }

            if (m_camZoomStarted)
            {
                m_camera->walkForward(dy);

                handled = true;
            }

            if (m_camPanStarted)
            {
                m_camera->strafeRight(-dx);
                m_camera->moveUpward(dy);

                handled = true;
            }

            m_lastMouseX = event.relativeXPosition;
            m_lastMouseY = event.relativeYPosition;

            if (handled)
            {
                return handled;
            }
        }

        case Core::MouseEventType::RA_MOUSE_RELEASED:
        {
            switch (event.button)
            {
                case Core::MouseButton::RA_MOUSE_LEFT_BUTTON:
                {
                    m_camRotateStarted = false;
                    return true;
                } break;

                case Core::MouseButton::RA_MOUSE_MIDDLE_BUTTON:
                {
                    m_camZoomStarted = false;
                    return true;
                } break;

                case Core::MouseButton::RA_MOUSE_RIGHT_BUTTON:
                {
                    m_camPanStarted = false;
                    return true;
                } break;
            }
        } break;

        case Core::MouseEventType::RA_MOUSE_WHEEL:
        {
            m_camera->walkForward( m_camera->getFocalPointDistance() * event.wheelDelta * 0.01 );//zoomIn(event.wheelDelta > 0 ? 0.1 : -0.1);
            m_camera->updateProjMatrix(m_width, m_height);
        }

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
            if (event.event == Core::KeyEventType::RA_KEY_RELEASED
                    && event.modifier & Core::Modifier::RA_CTRL_KEY)
            {
                ShaderProgramManager::getInstancePtr()->reloadAllShaderPrograms();
                return true;
            }
        } break;

        // Handle camera moving. Just an example.
        case Qt::Key_A:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->strafeLeft(0.1f);
                return true;
            }
        } break;

        case Qt::Key_D:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->strafeRight(0.1f);
                return true;
            }
        } break;

        case Qt::Key_W:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->walkForward(0.1f);
                return true;
            }
        } break;

        case Qt::Key_S:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->walkBackward(0.1f);
                return true;
            }
        } break;

        case Qt::Key_Up:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->rotateDown(Scalar(M_PI) / 50.0f);
                return true;
            }
        } break;

        case Qt::Key_Down:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->rotateUp(Scalar(M_PI) / 50.0f);
                return true;
            }
        } break;

        case Qt::Key_Left:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->rotateLeft(Scalar(M_PI) / 50.0f);
                return true;
            }
        } break;

        case Qt::Key_Right:
        {
            if (event.event == Core::KeyEventType::RA_KEY_PRESSED)
            {
                m_camera->rotateRight(Scalar(M_PI) / 50.0f);
                return true;
            }
        } break;
    }

    return false;
}

} // namespace Ra
