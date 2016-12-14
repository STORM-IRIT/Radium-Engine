#include <QTimer>
#include <QObject>
#include <QApplication>
#include <QCoreApplication>

#include <Core/Time/Timer.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/System/System.hpp>
#include <Engine/Entity/Entity.hpp>

#include <GuiBase/Viewer/Viewer.hpp>
#include <GuiBase/TimerData/FrameTimerData.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */


namespace {


    // Our minimal application uses QTimer to be called at a regular framerate.
    class MinimalApp : public QApplication {
    public:
        MinimalApp(int& argc, char** argv)
            : QApplication(argc, argv)
            , _engine     ( Ra::Engine::RadiumEngine::createInstance() )
            , _task_queue ( new Ra::Core::TaskQueue( std::thread::hardware_concurrency() -1 ) )
            , _frame_timer( nullptr )
            , _target_fps ( 60 )
        {

            _frame_timer = new QTimer( this );
            _frame_timer->setInterval( 1000 / _target_fps );
            connect( _frame_timer, &QTimer::timeout, this, &MinimalApp::frame );
        }

        ~MinimalApp() 
        {
            _engine->cleanup();
        }


    public slots:

    /// This function is called every frame and calls the engine and the renderer.
    void frame()
    {
        const Scalar dt = 1.f / Scalar( _target_fps );
        
        // Starts the renderer
        _viewer.startRendering(dt);

        // collect and run tasks
        _engine->getTasks( _task_queue.get(), dt );
        _task_queue->startTasks();
        _task_queue->waitForTasks();
        _task_queue->flushTaskQueue();

        // Finish the frame
        _viewer.waitForRendering();
        _viewer.update();
        _engine->endFrameSync();
    }


    public:
        // Our instance of the engine
        std::unique_ptr<Ra::Engine::RadiumEngine> _engine;

        // Task queue
        std::unique_ptr<Ra::Core::TaskQueue>      _task_queue;

        // Pointer to OpenGL Viewer for render call (belongs to MainWindow).
        Ra::Gui::Viewer _viewer;

        // Timer to wake us up at every frame start.
        QTimer* _frame_timer;

        // Our framerate
        uint _target_fps;

    }; // end class 
}

/// This is a very basic component which holds a spinning cube.
struct MinimalComponent : public Ra::Engine::Component
{

    MinimalComponent()
        : Ra::Engine::Component( "Minimal Component" )
    {}
    
    /// This function is called when the component is properly
    /// setup, i.e. it has an entity.
    void initialize() override
    {
        // Create a cube mesh render object.
        std::shared_ptr<Ra::Engine::Mesh> display (new Ra::Engine::Mesh( "Cube" ));
        display->loadGeometry( Ra::Core::MeshUtils::makeBox({0.1f,0.1f,0.1f}));
        auto renderObject = Ra::Engine::RenderObject::createRenderObject("CubeRO", this, Ra::Engine::RenderObjectType::Fancy, display);
        addRenderObject(renderObject);
    }

    /// This function will spin our cube
    void spin()
    {
        Ra::Core::AngleAxis aa( 0.01f, Ra::Core::Vector3::UnitY() );
        Ra::Core::Transform rot( aa );

        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject( m_renderObjects[0] );
        Ra::Core::Transform t = ro->getLocalTransform();
        ro->setLocalTransform( rot * t );
    }

};

/// The task will be called every frame to update the component,
/// here it will only call the spin() function.
struct MinimalTask : public Ra::Core::Task
{
    MinimalTask( Ra::Engine::Component * c ) : _comp( c ){}

    virtual std::string getName() const override
    {
        return "Minimal Task";
    }

    virtual void process() override
    {
        static_cast<MinimalComponent*>(_comp)->spin();
    }

    Ra::Engine::Component* _comp;
};

/// This system will be added to the engine. Every frame it will
/// add a MinimalTask to be executed.
class MinimalSystem : public Ra::Engine::System
{
public:
    virtual void generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) override
    {
        q->registerTask( new MinimalTask( m_components[0].second ) );
    }
};


int main ( int argc, char *argv[] )
{
    // A generic application
    // May be replaced by the old tabletapplication

    // Create app
    MinimalApp app ( argc, argv );
    app._engine->initialize();
    app._viewer.show();


    // Load Blinn-Phong shader
    Ra::Engine::ShaderConfiguration bpConfig("BlinnPhong");
    bpConfig.addShader( Ra::Engine::ShaderType_VERTEX, "../Shaders/BlinnPhong.vert.glsl" );
    bpConfig.addShader( Ra::Engine::ShaderType_FRAGMENT, "../Shaders/BlinnPhong.frag.glsl" );
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( bpConfig );

    // Create one system
    Ra::Engine::System * sys = new MinimalSystem;
    app._engine->registerSystem( "Minimal system", sys );

    // Create and initialize entity and component
    Ra::Engine::Entity* e = app._engine->getEntityManager()->createEntity( "Cube" );
    Ra::Engine::Component* c = new MinimalComponent;
    e->addComponent( c );
    sys->registerComponent( e, c );
    c->initialize();

    // Start the app.
    app._frame_timer->start();
    return app.exec();
}
