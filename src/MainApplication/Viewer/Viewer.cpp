#include <MainApplication/Viewer/Viewer.hpp>

#include <iostream>

#include <QTimer>
#include <QMouseEvent>
#include <QPainter>

#include <Core/String/StringUtils.hpp>
#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>

#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <MainApplication/Viewer/TrackballCamera.hpp>
#include <MainApplication/Viewer/Gizmo/GizmoManager.hpp>
#include <MainApplication/Gui/MainWindow.hpp>
#include <MainApplication/MainApplication.hpp>
#include <MainApplication/Utils/Keyboard.hpp>

/// Helper functions
namespace
{
    class RenderThread : public QThread, protected QOpenGLFunctions
    {
    public:
        RA_CORE_ALIGNED_NEW
        RenderThread( Ra::Gui::Viewer* viewer, Ra::Engine::Renderer* renderer )
            : QThread( viewer ), m_viewer( viewer ), m_renderer( renderer ), isInit( false )
        {
            CORE_ASSERT( m_renderer != nullptr && m_viewer != nullptr,
                         "Uninitialized renderer" );
        }

        virtual ~RenderThread() {}

        // This is the function that gets called in the render thread
        virtual void run() override
        {
            // check that the context has correctly been moved from the main thread.
            CORE_ASSERT( m_viewer->context()->thread() == QThread::currentThread(),
                         "Context is in the wrong thread" );

            // Grab the context
            m_viewer->makeCurrent();

            if ( !isInit )
            {
                initializeOpenGLFunctions();
                isInit = true;
            }

            CORE_ASSERT( glGetString( GL_VERSION ) != 0, "GL context unavailable" );

            // render will lock the renderer itself.
            m_renderer->render( m_renderData );

            // Give back viewer context to main thread.
            m_viewer->doneCurrent();
            m_viewer->context()->moveToThread( mainApp->thread() );
        }

        /// Keep a local copy of the render data.
        Ra::Engine::RenderData m_renderData;
        Ra::Gui::Viewer* m_viewer;
        Ra::Engine::Renderer* m_renderer;
        bool isInit;
    };

}

namespace Ra
{
    Gui::Viewer::Viewer( QWidget* parent )
        : QOpenGLWidget( parent )
        , m_gizmoManager(new GizmoManager(this))
        , m_renderThread( nullptr )
    {
        // Allow Viewer to receive events
        setFocusPolicy( Qt::StrongFocus );
        setMinimumSize( QSize( 800, 600 ) );

        m_camera.reset( new Gui::TrackballCamera( width(), height() ) );

        /// Intercept events to properly lock the renderer when it is compositing.
#if !defined(FORCE_RENDERING_ON_MAIN_THREAD)
        connect( this, &QOpenGLWidget::aboutToCompose, this, &Viewer::onAboutToCompose );
        connect( this, &QOpenGLWidget::frameSwapped,   this, &Viewer::onFrameSwapped );
        connect( this, &QOpenGLWidget::aboutToResize,  this, &Viewer::onAboutToResize );
        connect( this, &QOpenGLWidget::resized,        this, &Viewer::onResized );
#endif

    }

    Gui::Viewer::~Viewer()
    {
#if !defined(FORCE_RENDERING_ON_MAIN_THREAD)
        CORE_ASSERT( m_renderThread->isFinished(), "Render thread is still running" );
        delete m_renderThread;
#endif
    }

    void Gui::Viewer::initializeGL()
    {
        initializeOpenGLFunctions();

        LOG( logINFO ) << "*** Radium Engine Viewer ***";
        LOG( logINFO ) << "Renderer : " << glGetString( GL_RENDERER );
        LOG( logINFO ) << "Vendor   : " << glGetString( GL_VENDOR );
        LOG( logINFO ) << "OpenGL   : " << glGetString( GL_VERSION );
        LOG( logINFO ) << "GLSL     : " << glGetString( GL_SHADING_LANGUAGE_VERSION );

#if defined (OS_WINDOWS)
        glewExperimental = GL_TRUE;

        GLuint result = glewInit();
        if ( result != GLEW_OK )
        {
            std::string errorStr;
            Ra::Core::StringUtils::stringPrintf( errorStr, " GLEW init failed : %s", glewGetErrorString( result ) );
            CORE_ERROR( errorStr.c_str() );
        }
        else
        {
            LOG( logINFO ) << "GLEW     : " << glewGetString( GLEW_VERSION );
            glFlushError();
        }

#endif

#if defined(FORCE_RENDERING_ON_MAIN_THREAD)
        LOG( logDEBUG ) << "Rendering on main thread";
#else
        LOG( logDEBUG ) << "Rendering on dedicated thread";
#endif
        // FIXME(Charly): Renderer type should not be changed here
        m_renderers.resize( 1 );
        m_renderers[0].reset( new Engine::ForwardRenderer( width(), height() ) );

        for ( auto& renderer : m_renderers )
        {
            renderer->initialize();
        }

        m_currentRenderer = m_renderers[0].get();

#if !defined (FORCE_RENDERING_ON_MAIN_THREAD)
        m_renderThread = new RenderThread( this, m_renderer.get() );
#endif

        auto light = std::shared_ptr<Engine::DirectionalLight>(new Engine::DirectionalLight);

        for ( auto& renderer : m_renderers )
        {
            renderer->addLight( light );
        }

        m_camera->attachLight( light );

        emit rendererReady();
    }

    void Gui::Viewer::initRenderer()
    {
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

    void Gui::Viewer::resizeGL( int width, int height )
    {
        // Renderer should have been locked by previous events.
        m_camera->resizeViewport( width, height );
        m_currentRenderer->resize( width, height );
    }

    void Gui::Viewer::mousePressEvent( QMouseEvent* event )
    {
        switch ( event->button() )
        {
            case Qt::LeftButton:
            {
                if ( isKeyPressed( Key_Space ) )
                {
                    LOG( logINFO ) << "Raycast query launched";
                    Core::Ray r = m_camera->getCamera()->getRayFromScreen(Core::Vector2(event->x(), event->y()));
                    RA_DISPLAY_POINT(r.origin(), Core::Colors::Cyan(), 0.1f);
                    RA_DISPLAY_RAY(r, Core::Colors::Yellow());
                    auto ents = mainApp->getEngine()->getEntityManager()->getEntities();
                    for (auto e : ents)
                    {
                        e->rayCastQuery(r);
                    }
                }
                else
                {
                    Engine::Renderer::PickingQuery query  = { Core::Vector2(event->x(), height() - event->y()), Core::MouseButton::RA_MOUSE_LEFT_BUTTON };
                    m_currentRenderer->addPickingRequest(query);
                    m_gizmoManager->handleMousePressEvent(event);
                }
            }
            break;

            case Qt::MiddleButton:
            {
                m_camera->handleMousePressEvent(event);
            }
            break;

            case Qt::RightButton:
            {
                // Check picking
                Engine::Renderer::PickingQuery query  = { Core::Vector2(event->x(), height() - event->y()), Core::MouseButton::RA_MOUSE_RIGHT_BUTTON };
                m_currentRenderer->addPickingRequest(query);
            }
            break;

            default:
            {
            } break;
        }
    }

    void Gui::Viewer::mouseReleaseEvent( QMouseEvent* event )
    {
        m_camera->handleMouseReleaseEvent( event );
        m_gizmoManager->handleMouseReleaseEvent(event);
    }

    void Gui::Viewer::mouseMoveEvent( QMouseEvent* event )
    {
        m_camera->handleMouseMoveEvent( event );
        m_gizmoManager->handleMouseMoveEvent(event);
    }

    void Gui::Viewer::wheelEvent( QWheelEvent* event )
    {
        m_camera->handleWheelEvent(event);
        QOpenGLWidget::wheelEvent( event );
        mainApp->m_mainWindow->viewerWheelEvent( event );
    }

    void Gui::Viewer::keyPressEvent( QKeyEvent* event )
    {
        m_camera->handleKeyPressEvent( event );

        QOpenGLWidget::keyPressEvent(event);
    }

    void Gui::Viewer::keyReleaseEvent( QKeyEvent* event )
    {
        m_camera->handleKeyReleaseEvent( event );

        if (event->key() == Qt::Key_Z && !event->isAutoRepeat())
        {
            m_currentRenderer->toggleWireframe();
        }

        QOpenGLWidget::keyReleaseEvent(event);
    }

    void Gui::Viewer::reloadShaders()
    {
        // FIXME : check thread-saefty of this.
        m_currentRenderer->lockRendering();
        makeCurrent();
        m_currentRenderer->reloadShaders();
        doneCurrent();
        m_currentRenderer->unlockRendering();
    }

    void Gui::Viewer::displayTexture( const QString &tex )
    {
        m_currentRenderer->lockRendering();
        m_currentRenderer->displayTexture( tex.toStdString() );
        m_currentRenderer->unlockRendering();
    }

    void Gui::Viewer::changeRenderer( int index )
    {
        // NOTE(Charly): This is probably buggy since it has not been tested.
        LOG( logWARNING ) << "Changing renderers might be buggy since it has not been tested.";
        m_currentRenderer->lockRendering();
        m_currentRenderer = m_renderers[index].get();
        m_currentRenderer->initialize();
        m_currentRenderer->resize( width(), height() );
        m_currentRenderer->unlockRendering();
    }

    // Asynchronous rendering implementation

    void Gui::Viewer::startRendering( const Scalar dt )
    {
#if defined(FORCE_RENDERING_ON_MAIN_THREAD)
        makeCurrent();

        // Move camera if needed. Disabled for now as it takes too long (see issue #69)
        //m_camera->update( dt );

        Engine::RenderData data;
        data.dt = dt;
        data.projMatrix = m_camera->getProjMatrix();
        data.viewMatrix = m_camera->getViewMatrix();
        m_currentRenderer->render( data );
#else
        CORE_ASSERT( m_renderThread != nullptr,
                     "Render thread is not initialized (should have been done in initGL)" );

        // First release the context and give it to the rendering thread.
        doneCurrent();
        context()->moveToThread( m_renderThread );

        // Copy camera data from the main thread as some later events may overwrite it.
        Engine::RenderData& data = static_cast<RenderThread*>( m_renderThread )->m_renderData;
        data.projMatrix = m_camera->getProjMatrix();
        data.viewMatrix = m_camera->getViewMatrix();
        data.dt = dt;

        // Launch the thread, calling the run() method.
        m_renderThread->start();
#endif
    }

    void Gui::Viewer::waitForRendering()
    {
#if !defined(FORCE_RENDERING_ON_MAIN_THREAD)
        // Join with render thread.
        m_renderThread->wait();
        CORE_ASSERT( context()->thread() == QThread::currentThread(),
                     "Context has not been properly given back to main thread." );
        makeCurrent();
#endif
    }

    void Gui::Viewer::handleFileLoading( const std::string& file )
    {
        for ( auto& renderer : m_renderers )
        {
            renderer->handleFileLoading( file );
        }
    }

    void Gui::Viewer::processPicking()
    {
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
                emit rightClickPicking(m_currentRenderer->getPickingResults()[i]);
            }
        }
    }

    void Gui::Viewer::fitCameraToScene( const Core::Aabb& aabb )
    {
        // FIXME(Charly): Does not work, the camera needs to be fixed
        m_camera->fitScene( aabb );
    }

    std::vector<std::string> Gui::Viewer::getRenderersName() const
    {
        std::vector<std::string> ret;

        for ( const auto& r : m_renderers )
        {
            ret.push_back( r->getRendererName() );
        }

        return ret;
    }

    void Gui::Viewer::enablePostProcess(int enabled)
    {
        m_currentRenderer->enablePostProcess(enabled);
    }

} // namespace Ra
