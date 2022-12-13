#ifdef HEADLESS_HAS_EGL
#    include <Headless/OpenGLContext/EglOpenGLContext.hpp>

#    include <glbinding-aux/ValidVersions.h>
#    include <glbinding/glbinding.h>
#    include <globjects/globjects.h>

#    include <iostream>

// To prevent name collision on some systems between EGL and Eigen, those must be included last.
#    include <EGL/egl.h>
#    include <EGL/eglext.h>

namespace Ra {
namespace Headless {
using namespace gl;
using namespace glbinding;

static const EGLint configAttribs[] = { EGL_SURFACE_TYPE,
                                        EGL_PBUFFER_BIT,
                                        EGL_BLUE_SIZE,
                                        8,
                                        EGL_GREEN_SIZE,
                                        8,
                                        EGL_RED_SIZE,
                                        8,
                                        EGL_DEPTH_SIZE,
                                        8,
                                        EGL_RENDERABLE_TYPE,
                                        EGL_OPENGL_BIT,
                                        EGL_NONE };

struct EglOpenGLContext::ContextEGL {
    EGLDisplay m_eglDpy;
    EGLSurface m_eglSurf;
    EGLContext m_eglCtx;
};

EglOpenGLContext::EglOpenGLContext( const glbinding::Version& glVersion,
                                    const std::array<int, 2>& size ) :
    OpenGLContext( glVersion, size ), m_eglContext { new ContextEGL } {
    static const EGLint pbufferAttribs[] = {
        EGL_WIDTH,
        size[0], // not really needed, 1 should be OK
        EGL_HEIGHT,
        size[1], // not really needed, 1 should be OK
        EGL_NONE,
    };

    static const int MAX_DEVICES = 16;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;

    // 1. Initialize EGL
    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
        (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress( "eglQueryDevicesEXT" );
    eglQueryDevicesEXT( MAX_DEVICES, eglDevs, &numDevices );
    if ( numDevices == 0 ) {
        LOG( Core::Utils::logERROR ) << "EGL context creation : no devices available.";
        std::abort();
    }
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress( "eglGetPlatformDisplayEXT" );
    m_eglContext->m_eglDpy = eglGetPlatformDisplayEXT( EGL_PLATFORM_DEVICE_EXT, eglDevs[0], 0 );

    EGLint major, minor;
    if ( eglInitialize( m_eglContext->m_eglDpy, &major, &minor ) != EGL_TRUE ) {
        LOG( Core::Utils::logERROR ) << "EGL context creation : Unable to initialize EGL ";
        std::abort();
    }

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;
    if ( eglChooseConfig( m_eglContext->m_eglDpy, configAttribs, &eglCfg, 1, &numConfigs ) !=
         EGL_TRUE ) {
        LOG( Core::Utils::logERROR ) << "EGL context creation : Unable to choose EGL config";
        std::abort();
    }

    // 3. Create a surface
    // todo : do we really need a surface ???
    // see https://developer.nvidia.com/blog/egl-eye-opengl-visualization-without-x-server/
    m_eglContext->m_eglSurf =
        eglCreatePbufferSurface( m_eglContext->m_eglDpy, eglCfg, pbufferAttribs );
    if ( m_eglContext->m_eglSurf == EGL_NO_SURFACE ) {
        LOG( Core::Utils::logERROR ) << "EGL context creation : Unable to create pbuffer";
        std::abort();
    }

    // 4. Bind the API
    if ( eglBindAPI( EGL_OPENGL_API ) == EGL_FALSE ) {
        LOG( Core::Utils::logERROR ) << "EGL context creation : Unable to bing OpenGL API";
        std::abort();
    }

    // 5. Create a context and make it current
    EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION,
        glVersion.majorVersion(),
        EGL_CONTEXT_MINOR_VERSION,
        glVersion.minorVersion(),
        EGL_CONTEXT_OPENGL_PROFILE_MASK,
        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE,
    };
    m_eglContext->m_eglCtx =
        eglCreateContext( m_eglContext->m_eglDpy, eglCfg, EGL_NO_CONTEXT, contextAttribs );
    if ( m_eglContext->m_eglCtx == EGL_NO_CONTEXT ) {
        LOG( Core::Utils::logERROR ) << "EGL context creation : Unable to get context";
        std::abort();
    }

    // Initialize globjects (internally initializes glbinding, and registers the current
    // context)
    eglMakeCurrent( m_eglContext->m_eglDpy,
                    m_eglContext->m_eglSurf,
                    m_eglContext->m_eglSurf,
                    m_eglContext->m_eglCtx );
    globjects::init( []( const char* name ) { return eglGetProcAddress( name ); } );
}
EglOpenGLContext::~EglOpenGLContext() {
    eglTerminate( m_eglContext->m_eglDpy );
}
void EglOpenGLContext::makeCurrent() const {
    if ( m_eglContext->m_eglCtx ) {
        eglMakeCurrent( m_eglContext->m_eglDpy,
                        m_eglContext->m_eglSurf,
                        m_eglContext->m_eglSurf,
                        m_eglContext->m_eglCtx );
    }
}

void EglOpenGLContext::doneCurrent() const {
    if ( m_eglContext->m_eglCtx ) {
        eglMakeCurrent( m_eglContext->m_eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
    }
}

bool EglOpenGLContext::isValid() const {
    return m_eglContext->m_eglCtx != EGL_NO_CONTEXT;
}

std::string EglOpenGLContext::getInfo() const {
    auto generalInfo = OpenGLContext::getInfo();
    std::stringstream infoText;
    infoText << generalInfo << "Context provider     : EGL\n";
    return infoText.str();
}

} // namespace Headless
} // namespace Ra
#endif
