#include <glbinding/Binding.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>
// Do not import namespace to prevent glbinding/QTOpenGL collision
#include <glbinding/gl/gl.h>

#include <globjects/globjects.h>

#include <Engine/RadiumEngine.hpp>

#include <GuiBase/Viewer/Viewer.hpp>

#include <iostream>

#include <QOpenGLContext>

#include <QTimer>
#include <QMouseEvent>
#include <QPainter>

#include <Core/String/StringUtils.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Image/stb_image_write.h>

#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <GuiBase/Viewer/TrackballCamera.hpp>

#include <GuiBase/Utils/FeaturePickingManager.hpp>
#include <GuiBase/Utils/Keyboard.hpp>
#include <GuiBase/Utils/KeyMappingManager.hpp>

namespace Ra
{
    Gui::Viewer::Viewer( QScreen * screen )
        : QWindow(screen)
        , m_context(nullptr)
        , m_currentRenderer( nullptr )
        , m_featurePickingManager( nullptr )
        , m_camera( nullptr )
        , m_gizmoManager( nullptr )
        , m_renderThread( nullptr )
        , m_glInitStatus( false )
    {
        setMinimumSize( QSize( 800, 600 ) );

        setSurfaceType(OpenGLSurface);
        m_featurePickingManager = new FeaturePickingManager();
    }

    Gui::Viewer::~Viewer(){
        if ( m_glInitStatus.load() )
        {
            m_context->makeCurrent( this );
            m_renderers.clear();

            if (m_gizmoManager != nullptr)
            {
                delete m_gizmoManager;
            }
            m_context->doneCurrent( );
        }
    }

    void Gui::Viewer::createGizmoManager()
    {
        if (m_gizmoManager == nullptr)
        {
            m_gizmoManager = new GizmoManager(this);
        }
    }

    int Gui::Viewer::addRenderer(std::shared_ptr<Engine::Renderer> e){
        // initial state and lighting (deferred if GL is not ready yet)
        if ( m_glInitStatus.load() )
        {
            m_context->makeCurrent( this );
            intializeRenderer(e.get());
            m_context->doneCurrent( );
        }
        else
        {
            LOG( logINFO ) << "[Viewer] New Renderer ("
                           << e->getRendererName()
                           << ") added before GL being Ready: deferring initialization...";
        }

        m_renderers.push_back(e);

        if (m_currentRenderer == nullptr)
        {
            m_currentRenderer = e.get();
        }
        return m_renderers.size()-1;
    }


    void Gui::Viewer::enableDebug()
    {
        glbinding::setCallbackMask(glbinding::CallbackMask::After | glbinding::CallbackMask::ParametersAndReturnValue);
        glbinding::setAfterCallback([](const glbinding::FunctionCall & call)
                                    {
                                        std::cerr << call.function->name() << "(";
                                        for (unsigned i = 0; i < call.parameters.size(); ++i)
                                        {
                                            std::cerr << call.parameters[i]->asString();
                                            if (i < call.parameters.size() - 1)
                                            {
                                                std::cerr << ", ";
                                            }
                                        }
                                        std::cerr << ")";
                                        if (call.returnValue)
                                        {
                                            std::cerr << " -> " << call.returnValue->asString();
                                        }
                                        std::cerr << std::endl;
                                    });
    }

    void Gui::Viewer::initializeGL()
    {
//        LOG( logDEBUG ) << "Gui::Viewer::initializeGL : "  << width() << 'x' << height() << std::endl;
        // verify if context is created ?
        m_context->makeCurrent(this);

        m_camera.reset( new Gui::TrackballCamera( width(), height() ) );

        LOG( logINFO ) << "*** Radium Engine Viewer ***";
        Engine::ShaderProgramManager::createInstance("Shaders/Default.vert.glsl",
                                                     "Shaders/Default.frag.glsl");

        auto light = Ra::Core::make_shared<Engine::DirectionalLight>();
        m_camera->attachLight( light );


        // initialize renderers added before GL was ready
        if( ! m_renderers.empty() )
        {
            for ( auto& rptr : m_renderers )
            {
                intializeRenderer( rptr.get() );
                LOG( logINFO ) << "[Viewer] Deferred initialization of "
                               << rptr->getRendererName();
            }
            changeRenderer(0);
        }

        m_glInitStatus = true;

        m_context->doneCurrent();
        emit glInitialized();

    }

    Gui::CameraInterface* Gui::Viewer::getCameraInterface()
    {
        return m_camera.get();
    }

    Gui::GizmoManager* Gui::Viewer::getGizmoManager()
    {
        return m_gizmoManager;
    }

    const Engine::Renderer* Gui::Viewer::getRenderer() const
    {
        return m_currentRenderer;
    }

    Engine::Renderer* Gui::Viewer::getRenderer()
    {
        return m_currentRenderer;
    }

    Gui::FeaturePickingManager* Gui::Viewer::getFeaturePickingManager()
    {
        return m_featurePickingManager;
    }

    void Gui::Viewer::onAboutToCompose()
    {
        // This slot function is called from the main thread as part of the event loop
        // when the GUI is about to update. We have to wait for the rendering to finish.
        m_currentRenderer->lockRendering();
    }

    void Gui::Viewer::onFrameSwapped()
    {
        // This slot is called from the main thread as part of the event loop when the
        // GUI has finished displaying the rendered image, so we unlock the renderer.
        m_currentRenderer->unlockRendering();
    }

    void Gui::Viewer::onAboutToResize()
    {
        // Like swap buffers, resizing is a blocking operation and we have to wait for the rendering
        // to finish before resizing.
        m_currentRenderer->lockRendering();
    }

    void Gui::Viewer::onResized()
    {
        m_currentRenderer->unlockRendering();
    }

    void Gui::Viewer::intializeRenderer(Engine::Renderer *renderer)
    {
        // see issue #261 Qt Event order and default viewport management (Viewer.cpp)
        // https://github.com/STORM-IRIT/Radium-Engine/issues/261
#ifndef OS_MACOS
        gl::glViewport(0, 0, width(), height());
#endif
        renderer->initialize(width(), height());
        if( m_camera->hasLightAttached() )
        {
            renderer->addLight( m_camera->getLight() );
        }
    }

    void Gui::Viewer::resizeGL( int width_, int height_ )
    {
        if (isExposed())
        {
            // Renderer should have been locked by previous events.
            m_context->makeCurrent(this);

            // see issue #261 Qt Event order and default viewport management (Viewer.cpp)
            // https://github.com/STORM-IRIT/Radium-Engine/issues/261
#ifndef OS_MACOS
            gl::glViewport(0, 0, width(), height());
#endif
            m_camera->resizeViewport(width_, height_);
            m_currentRenderer->resize(width_, height_);
            m_context->doneCurrent();
        }
    }

    Engine::Renderer::PickingMode getPickingMode()
    {
        auto keyMap = Gui::KeyMappingManager::getInstance();
        if( Gui::isKeyPressed( keyMap->getKeyFromAction( Gui::KeyMappingManager::FEATUREPICKING_VERTEX ) ) )
        {
            return Engine::Renderer::VERTEX;
        }
        if( Gui::isKeyPressed( keyMap->getKeyFromAction( Gui::KeyMappingManager::FEATUREPICKING_EDGE ) ) )
        {
            return Engine::Renderer::EDGE;
        }
        if( Gui::isKeyPressed( keyMap->getKeyFromAction( Gui::KeyMappingManager::FEATUREPICKING_TRIANGLE ) ) )
        {
            return Engine::Renderer::TRIANGLE;
        }
        return Engine::Renderer::RO;
    }

    void Gui::Viewer::mousePressEvent( QMouseEvent* event )
    {
        if(! m_glInitStatus.load())
        {
            event->ignore();
            return;
        }

        auto keyMap = Gui::KeyMappingManager::getInstance();
        if( keyMap->actionTriggered( event, Gui::KeyMappingManager::VIEWER_LEFT_BUTTON_PICKING_QUERY ) )
        {
            if ( isKeyPressed( keyMap->getKeyFromAction(Gui::KeyMappingManager::VIEWER_RAYCAST_QUERY ) ) )
            {
                LOG( logINFO ) << "Raycast query launched";
                Core::Ray r = m_camera->getCamera()->getRayFromScreen(Core::Vector2(event->x(), event->y()));
                RA_DISPLAY_POINT(r.origin(), Core::Colors::Cyan(), 0.1f);
                RA_DISPLAY_RAY(r, Core::Colors::Yellow());
                auto ents = Engine::RadiumEngine::getInstance()->getEntityManager()->getEntities();
                for (auto e : ents)
                {
                    e->rayCastQuery(r);
                }
            }
            else
            {
                m_currentRenderer->addPickingRequest({ Core::Vector2(event->x(), height() - event->y()),
                                                       Core::MouseButton::RA_MOUSE_LEFT_BUTTON,
                                                       Engine::Renderer::RO });
                if (m_gizmoManager != nullptr)
                {
                    m_gizmoManager->handleMousePressEvent(event);
                }
            }
        }
        else if ( keyMap->actionTriggered( event, Gui::KeyMappingManager::TRACKBALLCAMERA_MANIPULATION ) )
        {
            m_camera->handleMousePressEvent(event);
        }
        else if ( keyMap->actionTriggered( event, Gui::KeyMappingManager::VIEWER_RIGHT_BUTTON_PICKING_QUERY ) )
        {
            // Check picking
            Engine::Renderer::PickingQuery query  = { Core::Vector2(event->x(), height() - event->y()),
                                                      Core::MouseButton::RA_MOUSE_RIGHT_BUTTON,
                                                      getPickingMode() };
            m_currentRenderer->addPickingRequest(query);
        }
    }

    void Gui::Viewer::mouseReleaseEvent( QMouseEvent* event )
    {
        m_camera->handleMouseReleaseEvent( event );
        if (m_gizmoManager != nullptr)
        {
            m_gizmoManager->handleMouseReleaseEvent(event);
        }
    }

    void Gui::Viewer::mouseMoveEvent( QMouseEvent* event )
    {
        if(m_glInitStatus.load())
        {
            m_camera->handleMouseMoveEvent( event );
            if (m_gizmoManager != nullptr)
                m_gizmoManager->handleMouseMoveEvent(event);
        }
        else
            event->ignore();
    }

    void Gui::Viewer::wheelEvent( QWheelEvent* event )
    {
        if(m_glInitStatus.load())
        {
            m_camera->handleWheelEvent(event);
        }
        else
        {
            event->ignore();
        }
    }

    void Gui::Viewer::keyPressEvent( QKeyEvent* event )
    {
        if(m_glInitStatus.load())
        {
            keyPressed(event->key());
            m_camera->handleKeyPressEvent( event );
        }
        else
        {
            event->ignore();
        }

        // Do we need this ?
        //QWindow::keyPressEvent(event);
    }

    void Gui::Viewer::keyReleaseEvent( QKeyEvent* event )
    {
        keyReleased(event->key());
        m_camera->handleKeyReleaseEvent( event );

        if ( Gui::KeyMappingManager::getInstance()->actionTriggered( event, Gui::KeyMappingManager::VIEWER_TOGGLE_WIREFRAME ) && !event->isAutoRepeat())
        {
            m_currentRenderer->toggleWireframe();
        }

        // Do we need this ?
        //QWindow::keyReleaseEvent(event);
    }

    void Gui::Viewer::resizeEvent(QResizeEvent *event)
    {
 //       LOG( logDEBUG ) << "Gui::Viewer --> Got resize event : "  << width() << 'x' << height();

        if(!m_glInitStatus.load())
        {
            initializeGL();
        }

        if (!m_currentRenderer || !m_camera)
            return;

        resizeGL(event->size().width(), event->size().height());
    }

    void Gui::Viewer::showEvent(QShowEvent *ev)
    {
 //       LOG( logDEBUG ) << "Gui::Viewer --> Got show event : " << width() << 'x' << height();
        if(!m_context)
        {
            m_context.reset(new QOpenGLContext());
            m_context->create();
            m_context->makeCurrent(this);
            // no need to initalize glbinding. globjects (magically) do this internally.
            globjects::init(globjects::Shader::IncludeImplementation::Fallback);

            LOG( logINFO ) << "*** Radium Engine OpenGL context ***";
            LOG( logINFO ) << "Renderer (glbinding) : " << glbinding::ContextInfo::renderer();
            LOG( logINFO ) << "Vendor   (glbinding) : " << glbinding::ContextInfo::vendor();
            LOG( logINFO ) << "OpenGL   (glbinding) : " << glbinding::ContextInfo::version().toString();
            LOG( logINFO ) << "GLSL                 : " << gl::glGetString(gl::GLenum(GL_SHADING_LANGUAGE_VERSION));

            m_context->doneCurrent();
        }
    }

    void Gui::Viewer::exposeEvent(QExposeEvent *ev)
    {
 //       LOG( logDEBUG ) << "Gui::Viewer --> Got exposed event : " << width() << 'x' << height();
    }

    void Gui::Viewer::reloadShaders()
    {
        CORE_ASSERT(m_glInitStatus.load(),
                    "OpenGL needs to be initialized reload shaders.");

        // FIXME : check thread-saefty of this.
        m_currentRenderer->lockRendering();

        m_context->makeCurrent(this);
        m_currentRenderer->reloadShaders();
        m_context->doneCurrent();

        m_currentRenderer->unlockRendering();
    }

    void Gui::Viewer::displayTexture( const QString &tex )
    {
        CORE_ASSERT(m_glInitStatus.load(),
                    "OpenGL needs to be initialized to display textures.");
        m_context->makeCurrent(this);
        m_currentRenderer->lockRendering();
        m_currentRenderer->displayTexture( tex.toStdString() );
        m_currentRenderer->unlockRendering();
        m_context->doneCurrent();
    }

    bool Gui::Viewer::changeRenderer( int index )
    {
        if (m_glInitStatus.load() && m_renderers[index])
        {
            m_context->makeCurrent(this);

            if(m_currentRenderer != nullptr)
            {
                m_currentRenderer->lockRendering();
            }

            m_currentRenderer = m_renderers[index].get();
            m_currentRenderer->resize( width(), height() );
            m_currentRenderer->unlockRendering();

            LOG( logINFO ) << "[Viewer] Set active renderer: "
                           << m_currentRenderer->getRendererName();

            emit rendererReady();

            m_context->doneCurrent();
            return true;
        }
        return false;
    }

    // Asynchronous rendering implementation

    void Gui::Viewer::startRendering( const Scalar dt )
    {
        CORE_ASSERT(m_glInitStatus.load(),
                    "OpenGL needs to be initialized before rendering.");

        CORE_ASSERT(m_currentRenderer != nullptr,
                    "No renderer found.");

        m_context->makeCurrent(this);

        // Move camera if needed. Disabled for now as it takes too long (see issue #69)
        //m_camera->update( dt );

        Engine::RenderData data;
        data.dt = dt;
        data.projMatrix = m_camera->getProjMatrix();
        data.viewMatrix = m_camera->getViewMatrix();

        m_currentRenderer->render( data );

    }

    void Gui::Viewer::waitForRendering()
    {
        if (isExposed())
        {
            m_context->swapBuffers(this);
        }

        m_context->doneCurrent();
    }

    void Gui::Viewer::handleFileLoading( const std::string& file )
    {
        for ( auto& renderer : m_renderers )
        {
            if (renderer)
            {
                renderer->handleFileLoading( file );
            }
        }
    }

    void Gui::Viewer::handleFileLoading(const Ra::Asset::FileData &filedata) {
        for ( auto& renderer : m_renderers )
        {
            if (renderer)
            {
                renderer->handleFileLoading( filedata );
            }
        }
    }

    void Gui::Viewer::processPicking()
    {
        CORE_ASSERT(m_glInitStatus.load(),
                    "OpenGL needs to be initialized before rendering.");

        CORE_ASSERT(m_currentRenderer != nullptr,
                    "No renderer found.");

        CORE_ASSERT( m_currentRenderer->getPickingQueries().size() == m_currentRenderer->getPickingResults().size(),
                    "There should be one result per query." );

        for (uint i = 0 ; i < m_currentRenderer->getPickingQueries().size(); ++i)
        {
            const Engine::Renderer::PickingQuery& query  = m_currentRenderer->getPickingQueries()[i];
            if ( query.m_button == Core::MouseButton::RA_MOUSE_LEFT_BUTTON)
            {
                emit leftClickPicking(m_currentRenderer->getPickingResults()[i]);
            }
            else if (query.m_button == Core::MouseButton::RA_MOUSE_RIGHT_BUTTON)
            {
                const int roIdx = m_currentRenderer->getPickingResults()[i];
                const Core::Ray ray = m_camera->getCamera()->getRayFromScreen({query.m_screenCoords(0), height()-query.m_screenCoords(1)});
                // FIXME: this is safe as soon as there is no "queued connection" related to the signal
                m_featurePickingManager->doPicking(roIdx, query, ray);
                emit rightClickPicking(roIdx);
            }
        }
    }

    void Gui::Viewer::fitCameraToScene( const Core::Aabb& aabb )
    {
        if (!aabb.isEmpty())
        {
            CORE_ASSERT(m_camera != nullptr, "No camera found.");
            m_camera->fitScene(aabb);
        }
        else
        {
            LOG( logINFO ) << "Unable to fit the camera to the scene : empty Bbox.";
        }
    }

    std::vector<std::string> Gui::Viewer::getRenderersName() const
    {
        std::vector<std::string> ret;

        for ( const auto& renderer : m_renderers )
        {
            if (renderer)
            {
                ret.push_back( renderer->getRendererName() );
            }
        }

        return ret;
    }

    void Gui::Viewer::grabFrame( const std::string& filename )
    {
        m_context->makeCurrent(this);

        uint w, h;
        uchar* writtenPixels = m_currentRenderer->grabFrame(w, h);

        std::string ext = Core::StringUtils::getFileExt(filename);

        if (ext == "bmp")
        {
            stbi_write_bmp(filename.c_str(), w, h, 4, writtenPixels);
        }
        else if (ext == "png")
        {
            stbi_write_png(filename.c_str(), w, h, 4, writtenPixels, w * 4 * sizeof(uchar));
        }
        else
        {
            LOG(logWARNING) << "Cannot write frame to "<<filename<<" : unsupported extension";
        }

        m_context->doneCurrent();

        delete[] writtenPixels;

    }

    void Gui::Viewer::enablePostProcess(int enabled)
    {
        m_currentRenderer->enablePostProcess(enabled);
    }

    void Gui::Viewer::enableDebugDraw(int enabled)
    {
        m_currentRenderer->enableDebugDraw(enabled);
    }

    void Gui::Viewer::resetCamera()
    {
        m_camera.reset( new Gui::TrackballCamera( width(), height() ) );
    }

} // namespace Ra
