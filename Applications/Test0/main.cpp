#include <QApplication>
#include <QTimer>


#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>

#include <minimalradium.hpp>
/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

    /// Our minimal application uses QTimer to be called at a regular frame rate.
    class MinimalApp : public QApplication
    {
    public:
        MinimalApp(int& argc, char** argv)
                : QApplication(argc, argv), _engine(Ra::Engine::RadiumEngine::createInstance()),
                  _task_queue(new Ra::Core::TaskQueue(std::thread::hardware_concurrency() - 1)), _frame_timer(nullptr),
                  _target_fps(60)
        {

            _frame_timer = new QTimer(this);
            _frame_timer->setInterval(1000 / _target_fps);
            connect(_frame_timer, &QTimer::timeout, this, &MinimalApp::frame);
        }

        ~MinimalApp()
        {
            _engine->cleanup();
        }


    public slots:

        /// This function is the basic "game loop" iteration of the engine.
        /// It starts the rendering then advance all systems by one frame.
        void frame()
        {
            // We use a fixed time step, but it is also possible
            // to check the time from last frame.
            const Scalar dt = 1.f / Scalar(_target_fps);

            // Starts the renderer
            _viewer.startRendering(dt);

            // Collect and run tasks
            _engine->getTasks(_task_queue.get(), dt);
            _task_queue->startTasks();
            _task_queue->waitForTasks();
            _task_queue->flushTaskQueue();

            // Finish the frame
            _viewer.waitForRendering();
            //_viewer.update();
            _viewer.repaint();
            _engine->endFrameSync();
        }

    public:
        // Our instance of the engine
        std::unique_ptr<Ra::Engine::RadiumEngine> _engine;

        // Task queue
        std::unique_ptr<Ra::Core::TaskQueue> _task_queue;

        // Pointer to Qt/OpenGL Viewer widget.
        Ra::Gui::Viewer _viewer;

        // Timer to wake us up at every frame start.
        QTimer* _frame_timer;

        // Our framerate
        uint _target_fps;

    }; // end class



int main(int argc, char* argv[])
{

    // Create default format for Qt.
    QSurfaceFormat format;
    format.setVersion( 4, 1 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    //format.setSamples( 16 );
    format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
    format.setSwapInterval( 0 );
    QSurfaceFormat::setDefaultFormat( format );

    // Create app
    MinimalApp app(argc, argv);
    app._engine->initialize();
    app._viewer.show();

    // Load Blinn-Phong shader
    Ra::Engine::ShaderConfiguration bpConfig("BlinnPhong");
    bpConfig.addShader(Ra::Engine::ShaderType_VERTEX, "Shaders/BlinnPhong.vert.glsl");
    bpConfig.addShader(Ra::Engine::ShaderType_FRAGMENT, "Shaders/BlinnPhong.frag.glsl");
    Ra::Engine::ShaderConfigurationFactory::addConfiguration(bpConfig);

    // Create one system
    Ra::Engine::System* sys = new MinimalSystem;
    app._engine->registerSystem("Minimal system", sys);

    // Create and initialize entity and component
    Ra::Engine::Entity* e = app._engine->getEntityManager()->createEntity("Cube");
    Ra::Engine::Component* c = new MinimalComponent;
    e->addComponent(c);
    sys->registerComponent(e, c);
    c->initialize();

    // Start the app.
    app._frame_timer->start();
    return app.exec();
}
