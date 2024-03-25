#ifdef HEADLESS_HAS_GLFW
#    include <Headless/OpenGLContext/GlfwOpenGLContext.hpp>

#    include <GLFW/glfw3.h>

#    include <glbinding-aux/ValidVersions.h>
#    include <glbinding/glbinding.h>
#    include <globjects/globjects.h>

#    include <iostream>

namespace Ra {
namespace Headless {
using namespace gl;
using namespace glbinding;

static void error( int errnum, const char* errmsg ) {
    std::cerr << "GlfwOpenGLContext::GLFW error -- " << "0x" << std::hex << errnum << std::dec
              << ": " << errmsg << std::endl;
}

GlfwOpenGLContext::GlfwOpenGLContext( const glbinding::Version& glVersion,
                                      const std::array<int, 2>& size ) :
    OpenGLContext( glVersion, size ) {
    // initialize openGL
    if ( glfwInit() ) {
        glfwSetErrorCallback( error );
        glfwDefaultWindowHints();
        glfwWindowHint( GLFW_VISIBLE, false );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, glVersion.majorVersion() );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, glVersion.minorVersion() );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, true );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        m_glfwContext =
            glfwCreateWindow( size[0], size[1], "Radium CommandLine Context", nullptr, nullptr );
    }
    const char* description;
    int code = glfwGetError( &description );
    if ( code == GLFW_VERSION_UNAVAILABLE ) {
        // if the requested opengl version is not available, try to get the recommended version
        // (4.1)
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
        m_glfwContext =
            glfwCreateWindow( size[0], size[1], "Radium CommandLine Context", nullptr, nullptr );
        code = glfwGetError( &description );
    }
    if ( code != GLFW_NO_ERROR ) {
        std::cerr << "OpenGL context creation failed. Terminate execution." << std::endl;
        error( code, description );
        glfwTerminate();
        std::exit( -1 );
    }
    else {
        // Initialize globjects (internally initializes glbinding, and registers the current
        // context)
        glfwMakeContextCurrent( m_glfwContext );
        globjects::init( []( const char* name ) { return glfwGetProcAddress( name ); } );
        glfwSetWindowUserPointer( m_glfwContext, this );
        auto resizeCB = []( GLFWwindow* window, int width, int height ) {
            auto context = static_cast<GlfwOpenGLContext*>( glfwGetWindowUserPointer( window ) );
            context->resizeFrameBuffer( width, height );
        };
        glfwSetFramebufferSizeCallback( m_glfwContext, resizeCB );
        auto keyCB = []( GLFWwindow* window, int key, int scancode, int action, int mods ) {
            auto context = static_cast<GlfwOpenGLContext*>( glfwGetWindowUserPointer( window ) );
            context->keyboardEventCallback( key, scancode, action, mods );
        };
        glfwSetKeyCallback( m_glfwContext, keyCB );

        auto mouseCB = []( GLFWwindow* window, int button, int action, int mods ) {
            auto context = static_cast<GlfwOpenGLContext*>( glfwGetWindowUserPointer( window ) );
            // see https://www.glfw.org/docs/latest/window_guide.html#window_scale
            float xscale, yscale;
            glfwGetWindowContentScale( window, &xscale, &yscale );
            // seems that the scale is not to be taken into account
            double xpos, ypos;
            glfwGetCursorPos( window, &xpos, &ypos );
            context->mouseEventCallback( button, action, mods, int( xpos ), int( ypos ) );
        };
        glfwSetMouseButtonCallback( m_glfwContext, mouseCB );

        auto scrollCB = []( GLFWwindow* window, double xoffset, double yoffset ) {
            auto context = static_cast<GlfwOpenGLContext*>( glfwGetWindowUserPointer( window ) );
            float xscale, yscale;
            glfwGetWindowContentScale( window, &xscale, &yscale );
            context->scrollEventCallback( int( xoffset ), int( yoffset ) );
        };
        glfwSetScrollCallback( m_glfwContext, scrollCB );

        auto mouseMoveCB = []( GLFWwindow* window, double xpos, double ypos ) {
            auto context = static_cast<GlfwOpenGLContext*>( glfwGetWindowUserPointer( window ) );
            context->mouseMoveEventCallback( int( xpos ), int( ypos ) );
        };
        glfwSetCursorPosCallback( m_glfwContext, mouseMoveCB );
    }
}
GlfwOpenGLContext::~GlfwOpenGLContext() {
    glfwTerminate();
}
void GlfwOpenGLContext::makeCurrent() const {
    if ( m_glfwContext ) { glfwMakeContextCurrent( m_glfwContext ); }
}

void GlfwOpenGLContext::doneCurrent() const {
    if ( m_glfwContext ) { glfwMakeContextCurrent( nullptr ); }
}

bool GlfwOpenGLContext::isValid() const {
    return m_glfwContext != nullptr;
}

std::string GlfwOpenGLContext::getInfo() const {
    auto generalInfo = OpenGLContext::getInfo();
    std::stringstream infoText;
    infoText << generalInfo << "Context provider     : GLFW\n";
    return infoText.str();
}

void GlfwOpenGLContext::show( EventMode mode, float delay ) {
    m_mode  = mode;
    m_delay = delay;
    glfwShowWindow( m_glfwContext );
}

void GlfwOpenGLContext::hide() {
    glfwHideWindow( m_glfwContext );
}

void GlfwOpenGLContext::resize( const std::array<int, 2>& size ) {
    glfwSetWindowSize( m_glfwContext, size[0], size[1] );
}

bool GlfwOpenGLContext::processEvents() {
    switch ( m_mode ) {
    case EventMode::POLL:
        glfwPollEvents();
        break;
    case EventMode::WAIT:
        glfwWaitEvents();
        break;
    case EventMode::TIMEOUT:
        glfwWaitEventsTimeout( m_delay );
        break;
    default:
        glfwPollEvents();
        break;
    }
    return true;
}

void GlfwOpenGLContext::renderLoop( std::function<void( float )> render ) {
    double prevFrameDate = glfwGetTime();
    double curFrameDate;

    int width, height;
    glfwGetFramebufferSize( m_glfwContext, &width, &height );
    glViewport( 0, 0, width, height );

    while ( !glfwWindowShouldClose( m_glfwContext ) ) {
        curFrameDate = glfwGetTime();
        render( curFrameDate - prevFrameDate );
        prevFrameDate = curFrameDate;

        glfwSwapBuffers( m_glfwContext );
        processEvents();
    }
}

} // namespace Headless
} // namespace Ra
#endif
