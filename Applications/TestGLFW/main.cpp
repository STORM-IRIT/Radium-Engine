
#include <Core/CoreMacros.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>


#include <glbinding/gl/gl.h>
#include <globjects/globjects.h>

#include <Engine/RadiumEngine.hpp>

#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Component.hpp>
#include <Engine/System/System.hpp>

#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <GLFW/glfw3.h>

#include <nanogui/screen.h>
#include <nanogui/formhelper.h>




class MinimalComponent : public Ra::Engine::Component
{
public:
    MinimalComponent() : Ra::Engine::Component("Minimal Component") {}

    /// This function is called when the component is properly
    /// setup, i.e. it has an entity.
    void  initialize()  override
    {
        m_speed = 0.01f;
        // Create a cube mesh render object.
        std::shared_ptr<Ra::Engine::Mesh> display(new Ra::Engine::Mesh("Cube"));
        display->loadGeometry(Ra::Core::MeshUtils::makeBox({0.1f, 0.1f, 0.1f}));
        auto renderObject = Ra::Engine::RenderObject::createRenderObject("CubeRO", this,
                                                                         Ra::Engine::RenderObjectType::Fancy,
                                                                         display);
        addRenderObject(renderObject);
    }

    /// This function will spin our cube
    void  spin()
    {
        Ra::Core::AngleAxis aa(m_speed, Ra::Core::Vector3::UnitY());
        Ra::Core::Transform rot(aa);

        auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
                    m_renderObjects[0]);
        Ra::Core::Transform t = ro->getLocalTransform();
        ro->setLocalTransform(rot * t);
    }

    Scalar m_speed ;
};

/// This system will be added to the engine. Every frame it will
/// add a task to be executed, calling the spin function of the component.
class MinimalSystem : public Ra::Engine::System
{
public:
    void generateTasks(Ra::Core::TaskQueue *q, const Ra::Engine::FrameInfo &info) override
    {
        // We check that our component is here.
        CORE_ASSERT(m_components.size() == 1, "System incorrectly initialized");
        MinimalComponent *c = static_cast<MinimalComponent *>(m_components[0].second);

        // Create a new task which wil call c->spin() when executed.
        q->registerTask(new Ra::Core::FunctionTask(std::bind(&MinimalComponent::spin, c), "spin"));
    }
};

/// I/O processing
void processInput( GLFWwindow* window )
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window,true);
    }
}

/// These globals are necessary for resizing the viewport
struct RenderContext
{
    Ra::Engine::Renderer* renderer;
    Ra::Engine::Camera* camera;
    nanogui::Screen* screen;
};
RenderContext g_context;


int main()
{

    // Default starting width and height
    const uint width = 800;
    const uint height = 600;

    // Initialize GLFW
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, 	GLFW_TRUE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow( width , height, "GLFW test", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        CORE_ERROR("Failwd to create GLFW window");
    }

    // Initialize Radium
    std::unique_ptr<Ra::Engine::RadiumEngine> engine(Ra::Engine::RadiumEngine::createInstance());
    engine->initialize();
    std::unique_ptr<Ra::Core::TaskQueue> taskQueue ( new Ra::Core::TaskQueue(std::thread::hardware_concurrency() -1 ));

    // Load Blinn-Phong shader
    Ra::Engine::ShaderConfiguration bpConfig("BlinnPhong");
    bpConfig.addShader(Ra::Engine::ShaderType_VERTEX, "Shaders/BlinnPhong.vert.glsl");
    bpConfig.addShader(Ra::Engine::ShaderType_FRAGMENT, "Shaders/BlinnPhong.frag.glsl");
    Ra::Engine::ShaderConfigurationFactory::addConfiguration(bpConfig);

    Ra::Engine::ShaderConfiguration pConfig("Plain");
    pConfig.addShader(Ra::Engine::ShaderType_VERTEX, "Shaders/Plain.vert.glsl");
    pConfig.addShader(Ra::Engine::ShaderType_FRAGMENT, "Shaders/Plain.frag.glsl");
    Ra::Engine::ShaderConfigurationFactory::addConfiguration(pConfig);

    // Initialize Renderer
    glfwMakeContextCurrent(window);
    globjects::init(globjects::Shader::IncludeImplementation::Fallback);

    std::unique_ptr<Ra::Engine::Camera> camera ( new Ra::Engine::Camera(height, width));
    Ra::Engine::ShaderProgramManager::createInstance("Shaders/Default.vert.glsl",
                                                     "Shaders/Default.frag.glsl");
    std::unique_ptr<Ra::Engine::Renderer> renderer(new Ra::Engine::ForwardRenderer(width, height));
    renderer->initialize();




    // Initialize scene
    camera->setPosition({0,1,1});
    camera->setDirection({0,-1,-1});

    Ra::Engine::System* sys = new MinimalSystem;
    engine->registerSystem("Minimal system", sys);
    Ra::Engine::Entity* e = engine->getEntityManager()->createEntity("Cube");
    MinimalComponent* c = new MinimalComponent;
    e->addComponent(c);
    sys->registerComponent(e, c);
    c->initialize();


    // Initialize nanogui
    nanogui::Screen* screen = new nanogui::Screen();
    screen->initialize(window, true);

    nanogui::FormHelper* gui = new nanogui::FormHelper(screen);
    nanogui::ref<nanogui::Window>ngWindow = gui->addWindow({10,10}, "Gui");
    gui->addVariable("speed", c->m_speed)->setSpinnable(true);

    screen->setVisible(true);
    screen->performLayout();
    ngWindow->center();


    glfwSetCursorPosCallback(window,
                             [](GLFWwindow *, double x, double y) {
        g_context.screen->cursorPosCallbackEvent(x, y);
    }
    );

    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow *, int button, int action, int modifiers) {
        g_context.screen->mouseButtonCallbackEvent(button, action, modifiers);
    }
    );

    glfwSetKeyCallback(window,
                       [](GLFWwindow *, int key, int scancode, int action, int mods) {
        g_context.screen->keyCallbackEvent(key, scancode, action, mods);
    }
    );

    glfwSetCharCallback(window,
                        [](GLFWwindow *, unsigned int codepoint) {
        g_context.screen->charCallbackEvent(codepoint);
    }
    );

    glfwSetDropCallback(window,
                        [](GLFWwindow *, int count, const char **filenames) {
        g_context.screen->dropCallbackEvent(count, filenames);
    }
    );

    glfwSetScrollCallback(window,
                          [](GLFWwindow *, double x, double y) {
        g_context.screen->scrollCallbackEvent(x, y);
    }
    );
    glfwSetFramebufferSizeCallback(window,
                                   [] (GLFWwindow* window, int width, int height) {
        g_context.renderer->resize(width, height);
        g_context.camera->resize(Scalar(width), Scalar(height));
        g_context.screen->resizeCallbackEvent(width, height);
    }
    );

    g_context.camera = camera.get();
    g_context.renderer= renderer.get();
    g_context.screen = screen;

    g_context.renderer->resize(width, height);
    g_context.camera->resize(Scalar(width), Scalar(height));
    g_context.screen->resizeCallbackEvent(width, height);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        const Scalar dt = 1.0f/30.f;

        double frame_start  = glfwGetTime();

        // I/O Processing
        glfwPollEvents();
        processInput(window);

        // Advance engine
        engine->getTasks(taskQueue.get(), dt);
        taskQueue->startTasks();
        taskQueue->waitForTasks();
        taskQueue->flushTaskQueue();

        // Rendering
        Ra::Engine::RenderData data;
        data.dt = dt;
        data.viewMatrix = camera->getViewMatrix();
        data.projMatrix = camera->getProjMatrix();
        renderer->render(data);

        screen->drawContents();
        screen->drawWidgets();

        // End of frame
        glfwSwapBuffers(window);
        engine->endFrameSync();

        // Spin to wait for next frame
        double frame_end;
        while ((frame_end = glfwGetTime()) - frame_start < dt )
        {}
    }

    // Clean up
    glfwTerminate();
    engine->cleanup();
    return 0;
}
